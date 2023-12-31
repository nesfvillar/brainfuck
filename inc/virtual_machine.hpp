#pragma once

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
        VirtualMachine(std::string_view const program, std::ostream& ostream, std::istream& istream)
            : program_(std::from_range, program
                | std::views::filter(_is_valid_opcode)
                | std::views::transform(_parse_opcode)),
            ostream_(ostream),
            istream_(istream) {}

        VirtualMachine() = delete;

        VirtualMachine(VirtualMachine const& vm)
            : program_(vm.program_),
            program_iterator_(vm.program_iterator_),
            memory_(vm.memory_),
            data_iterator_(vm.data_iterator_),
            ostream_(vm.ostream_),
            istream_(vm.istream_) {}

        VirtualMachine(VirtualMachine&& vm) noexcept
            : program_(std::move(vm.program_)),
            program_iterator_(std::move(vm.program_iterator_)),
            memory_(std::move(vm.memory_)),
            data_iterator_(std::move(vm.data_iterator_)),
            ostream_(vm.ostream_),
            istream_(vm.istream_) {}

        VirtualMachine& operator=(VirtualMachine const&) = delete;

        VirtualMachine& operator=(VirtualMachine&&) = delete;

        ~VirtualMachine() = default;

        void run(this auto&& self)
        {
            while (self.program_iterator_ < self.program_.size()) [[likely]]
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
        size_t program_iterator_ = 0;

        std::deque<int> memory_ = { 0 };
        size_t data_iterator_ = 0;

        std::ostream& ostream_;
        std::istream& istream_;

    private:
        bool static constexpr _is_valid_opcode(char const c)
        {
            std::string_view constexpr valid_opcodes{ "><+-.,[]" };
            return std::ranges::find(valid_opcodes, c) != valid_opcodes.end();
        }

        Instruction_ static constexpr _parse_opcode(char const c) noexcept
        {
            return static_cast<Instruction_>(c);
        }
    };
}
