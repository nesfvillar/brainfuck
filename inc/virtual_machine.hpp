#pragma once

#include <cassert>
#include <deque>
#include <exception>
#include <iostream>
#include <ranges>
#include <vector>

namespace bf
{
    class program_error : public std::exception
    {
    public:
        program_error() : std::exception("program error") {}
    };

    class VirtualMachine
    {
    public:
        VirtualMachine(std::string_view program) :
            program_(std::from_range, program
                | std::views::filter(_is_valid_opcode)
                | std::views::transform(_parse_opcode)) {}

        VirtualMachine(std::string_view program, std::ostream& ostream, std::istream& istream) :
            program_(std::from_range, program
                | std::views::filter(_is_valid_opcode)
                | std::views::transform(_parse_opcode)),
            ostream_(ostream),
            istream_(istream) {}

        VirtualMachine() = delete;

        VirtualMachine(VirtualMachine const& vm)
            : program_(vm.program_),
            program_iterator_(program_.begin() +
                static_cast<size_t>(vm.program_iterator_ - vm.program_.begin())),
            memory_(vm.memory_),
            data_iterator_(memory_.begin() +
                static_cast<size_t>(vm.data_iterator_ - vm.memory_.begin())),
            ostream_(vm.ostream_),
            istream_(vm.istream_) {}

        VirtualMachine(VirtualMachine&& vm) noexcept
            : program_(std::move(vm.program_)),
            program_iterator_(program_.begin() +
                static_cast<size_t>(vm.program_iterator_ - vm.program_.begin())),
            memory_(std::move(vm.memory_)),
            data_iterator_(memory_.begin() +
                static_cast<size_t>(vm.data_iterator_ - vm.memory_.begin())),
            ostream_(vm.ostream_),
            istream_(vm.istream_) {}

        VirtualMachine& operator=(VirtualMachine const&) = delete;

        VirtualMachine& operator=(VirtualMachine&&) = delete;

        constexpr ~VirtualMachine() noexcept = default;

        void run(this auto&& self)
        {
            while (self.program_iterator_ != self.program_.end()) [[likely]]
            {
                self.step();
            }
        }

    protected:
        enum class Instruction_ : char
        {
            IncrementPointer = '>',
            DecrementPointer = '<',
            Increment = '+',
            Decrement = '-',
            Output = '.',
            Input = ',',
            JumpForward = '[',
            JumpBackward = ']',
        };

        std::vector<Instruction_> const program_;
        std::vector<Instruction_>::const_iterator program_iterator_ = program_.begin();

        std::deque<int> memory_ = { 0 };
        std::deque<int>::iterator data_iterator_ = memory_.begin();

        std::ostream& ostream_ = std::cout;
        std::istream& istream_ = std::cin;

    private:
        bool static constexpr _is_valid_opcode(char const c)
        {
            constexpr std::string_view valid_opcodes{ "><+-.,[]" };
            return std::ranges::find(valid_opcodes, c) != valid_opcodes.end();
        }

        Instruction_ static constexpr _parse_opcode(char const c) noexcept
        {
            return static_cast<Instruction_>(c);
        }
    };
}