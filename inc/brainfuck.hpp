#pragma once

#include "virtual_machine.hpp"

#include <cassert>
#include <iostream>
#include <ranges>
#include <vector>

namespace bf
{
    class Brainfuck : public VirtualMachine
    {
    public:
        constexpr Brainfuck(std::string_view program)
            : VirtualMachine(program), _jump_table(std::move(_create_jump_table(program_))) {}

        constexpr Brainfuck(std::string_view program, std::ostream& ostream, std::istream& istream)
            : VirtualMachine(program, ostream, istream),
            _jump_table(std::move(_create_jump_table(program_))) {}

        constexpr Brainfuck(Brainfuck const& bf) noexcept
            : VirtualMachine(bf), _jump_table(bf._jump_table) {}

        constexpr Brainfuck(Brainfuck&& bf) noexcept
            : VirtualMachine(bf), _jump_table(std::move(bf._jump_table)) {}

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
                program_iterator_ = program_.begin() + _jump_table[program_iterator_ - program_.begin()];
            }
        }

        void _jump_backward() noexcept
        {
            if (*data_iterator_ != 0)
            {
                program_iterator_ = program_.begin() + _jump_table[program_iterator_ - program_.begin()];
            }
        }

        std::vector<size_t> static constexpr _create_jump_table(std::vector<Instruction_> const& program) noexcept(false)
        {
            std::vector<size_t> jump_table{ program.size() };

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
