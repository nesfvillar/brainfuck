#pragma once

#include "virtual_machine.hpp"

#include <unordered_map>

namespace bf
{
    class Brainfuck : public VirtualMachine
    {
    public:
        Brainfuck(std::string_view program)
            : VirtualMachine(program), _jump_map(std::move(_create_jump_map(program_))) {}

        Brainfuck(std::string_view program, std::ostream& ostream, std::istream& istream)
            : VirtualMachine(program, ostream, istream),
            _jump_map(std::move(_create_jump_map(program_))) {}

        Brainfuck() = delete;

        Brainfuck(Brainfuck const&) = default;

        Brainfuck(Brainfuck&&) noexcept = default;

        Brainfuck& operator=(Brainfuck const&) = delete;

        Brainfuck& operator=(Brainfuck&&) = delete;

        ~Brainfuck() noexcept = default;

        void step()
        {
            assert(program_iterator_ != program_.end());
            switch (*program_iterator_)
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
        void constexpr _increment_data_pointer() noexcept
        {
            data_iterator_++;
            assert(data_iterator_ != memory_.end());
        }

        void constexpr _decrement_data_pointer() noexcept
        {
            data_iterator_--;
            assert(data_iterator_ != memory_.end());
        }

        void constexpr _increment_data() noexcept
        {
            (*data_iterator_)++;
        }

        void constexpr _decrement_data() noexcept
        {
            (*data_iterator_)--;
        }

        void _output_data()
        {
            ostream_ << static_cast<char>(*data_iterator_);
        }

        void _input_data()
        {
            istream_ >> *data_iterator_;
        }

        void _jump_forward() noexcept
        {
            if (*data_iterator_ == 0)
            {
                program_iterator_ = program_.begin() + _jump_map.at(program_iterator_ - program_.begin());
            }
        }

        void _jump_backward() noexcept
        {
            if (*data_iterator_ != 0)
            {
                program_iterator_ = program_.begin() + _jump_map.at(program_iterator_ - program_.begin());
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
