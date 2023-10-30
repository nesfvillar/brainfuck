#pragma once

#include "virtual_machine.hpp"

#include <unordered_map>

namespace bf
{
    class Brainfuck : public VirtualMachine
    {
    public:
        Brainfuck(std::string_view const program)
            : VirtualMachine(program), _jump_map(std::move(_create_jump_map(program_))) {}

        Brainfuck(std::string_view const program, std::ostream& ostream, std::istream& istream)
            : VirtualMachine(program, ostream, istream),
            _jump_map(std::move(_create_jump_map(program_))) {}

        Brainfuck() = delete;

        Brainfuck(Brainfuck const& bf)
            : VirtualMachine(bf),
            _jump_map(bf._jump_map) {}

        Brainfuck(Brainfuck&& bf) noexcept
            : VirtualMachine(std::move(bf)),
            _jump_map(std::move(bf._jump_map)) {}

        Brainfuck& operator=(Brainfuck const&) = delete;

        Brainfuck& operator=(Brainfuck&&) = delete;

        ~Brainfuck() = default;

        void step()
        {
            switch (program_[program_iterator_])
            {
            case Instruction_::IncrementPointer:
                _increment_data_pointer();
                break;
            case Instruction_::DecrementPointer:
                _decrement_data_pointer();
                break;
            case Instruction_::Increment:
                _increment_data();
                break;
            case Instruction_::Decrement:
                _decrement_data();
                break;
            case Instruction_::Output:
                _output_data();
                break;
            case Instruction_::Input:
                _input_data();
                break;
            case Instruction_::JumpForward:
                _jump_forward();
                break;
            case Instruction_::JumpBackward:
                _jump_backward();
                break;
            default:
                std::unreachable();
            }
            program_iterator_++;
        }

    private:
        void _increment_data_pointer()
        {
            data_iterator_++;
            if (data_iterator_ == memory_.size()) [[unlikely]]
                {
                    memory_.push_back(0);
                }
        }

        void _decrement_data_pointer()
        {
            if (data_iterator_ == 0) [[unlikely]]
                {
                    memory_.push_front(0);
                }
            else [[likely]]
                {
                    data_iterator_--;
                }
        }

        void _increment_data() noexcept
        {
            memory_[data_iterator_]++;
        }

        void _decrement_data() noexcept
        {
            memory_[data_iterator_]--;
        }

        void _output_data()
        {
            ostream_ << static_cast<char>(memory_[data_iterator_]);
        }

        void _input_data()
        {
            istream_ >> memory_[data_iterator_];
        }

        void _jump_forward() noexcept
        {
            if (memory_[data_iterator_] == 0)
            {
                program_iterator_ = _jump_map.at(program_iterator_);
            }
        }

        void _jump_backward() noexcept
        {
            if (memory_[data_iterator_] != 0)
            {
                program_iterator_ = _jump_map.at(program_iterator_);
            }
        }

        std::unordered_map<size_t, size_t> static _create_jump_map(std::vector<Instruction_> const& program) noexcept(false)
        {
            std::unordered_map<size_t, size_t> jump_map;
            std::vector<size_t> jump_stack;

            for (size_t begin_idx{};
                auto const [idx, ins] : program | std::views::enumerate)
            {
                switch (ins)
                {
                case Instruction_::JumpForward:
                    jump_stack.push_back(idx);
                    break;
                case Instruction_::JumpBackward:
                    if (jump_stack.empty()) [[unlikely]] {
                        throw program_error{};
                        }

                    begin_idx = jump_stack.back();
                    jump_stack.pop_back();

                    jump_map[begin_idx] = idx;
                    jump_map[idx] = begin_idx;
                    break;
                [[likely]] default:
                    break;
                }
            }

            if (!jump_stack.empty())
            {
                throw program_error{};
            }

            return jump_map;
        }

        std::unordered_map<size_t, size_t> const _jump_map;
    };
}
