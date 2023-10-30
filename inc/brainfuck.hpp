#pragma once

#include "virtual_machine.hpp"

namespace bf
{
    class Brainfuck : public VirtualMachine
    {
    public:
        Brainfuck(std::string_view const program)
            : VirtualMachine(program), _jump_table(std::move(_create_jump_table(program_))) {}

        Brainfuck(std::string_view const program, std::ostream& ostream, std::istream& istream)
            : VirtualMachine(program, ostream, istream),
            _jump_table(std::move(_create_jump_table(program_))) {}

        Brainfuck() = delete;

        Brainfuck(Brainfuck const& bf)
            : VirtualMachine(bf),
            _jump_table(bf._jump_table) {}

        Brainfuck(Brainfuck&& bf) noexcept
            : VirtualMachine(std::move(bf)),
            _jump_table(std::move(bf._jump_table)) {}

        Brainfuck& operator=(Brainfuck const&) = delete;

        Brainfuck& operator=(Brainfuck&&) = delete;

        ~Brainfuck() noexcept = default;

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
                program_iterator_ = _jump_table[program_iterator_];
            }
        }

        void _jump_backward() noexcept
        {
            if (memory_[data_iterator_] != 0)
            {
                program_iterator_ = _jump_table[program_iterator_];
            }
        }

        std::vector<size_t> static constexpr _create_jump_table(std::vector<Instruction_> const& program) noexcept(false)
        {
            std::vector<size_t> jump_table(program.size());

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
                    begin_idx = jump_stack.back();
                    jump_stack.pop_back();

                    jump_table[begin_idx] = idx;
                    jump_table[idx] = begin_idx;
                    break;
                [[likely]] default:
                    break;
                }
            }

            if (!jump_stack.empty())
            {
                throw program_error{};
            }

            return jump_table;
        }

        std::vector<size_t> const _jump_table;
    };
}
