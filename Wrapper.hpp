#pragma once

#include <compare>
#include <concepts>
#include <ostream>
#include <utility>
#include <type_traits>
#include <memory>

/**
    @brief B-tree namespace
*/
namespace Btree {
    /**
        @brief Concept defining requirements for types that can be used with WrapperType
        @details Required poperties:
            * - Default initialization support
            * - Copy construction support
            * - Copy assignment support
            * - Move construction support
            * - Move assignment support
            * - <, >, ==, != comparison operations support(Total ordering)
            * << stream insertion operator support 
        @tparam T the type to check against requirements     
    */
    template <typename T>
    requires std::default_initializable<T> &&
                std::copy_constructible<T> &&
                std::move_constructible<T> &&
                std::assignable_from<T&, const T&> &&
                std::assignable_from<T&, T> &&
                std::totally_ordered<T> &&
                requires (std::ostream& os, const T& v) {
                    { os << v } -> std::convertible_to<std::ostream&>;
                }
    class   WrapperType final {
        public:

            /**
                @brief ctors, dtor, operator assignments
                @details These functions manage object lifecycle
                * - In each scenario, they work without exception, as the values of all properties are correctly taken into account. 
                * - Support for all semantics, assignment operators work with the type T itself
                * - Safe for implicit conversions
                * - Rule of six compliance
            */

            WrapperType() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
            WrapperType(const WrapperType<T>&) noexcept(std::is_nothrow_copy_constructible_v<T>) = default;
            WrapperType(WrapperType<T>&&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;
            ~WrapperType() = default;
            WrapperType<T>&   operator=(const WrapperType<T>&) noexcept(std::is_nothrow_copy_assignable_v<T>) = default;
            WrapperType<T>&   operator=(WrapperType<T>&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;
            
            explicit WrapperType(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
                : m_value(value) , m_isInstalled(true) {
            }
            explicit WrapperType(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
                : m_value(std::move(value)), m_isInstalled(true) {
            }
            
            WrapperType<T>&    operator=(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
                if (std::addressof(m_value) != std::addressof(value))
                    m_value = value;
                m_isInstalled = true;
                return *this;
            }

            WrapperType<T>&    operator=(T&& value) noexcept(std::is_nothrow_move_assignable_v<T>) {
                if (std::addressof(m_value) != std::addressof(value))
                    m_value = std::move(value);
                m_isInstalled = true;
                return *this;
            }

            /**
                @brief Operators overload
                @details These functions provide functionality for working with the type internally.
                    * - Convertion Wrapper to type T
                    * - Comparing operations with Wrappers or T type (<, >, ==, !=)
                    * - Aspirational (friend) comparative functional for additional use
                    * - stream insertion(operator<<) overload
            */
            
             explicit operator T() const noexcept {
                return m_value;
            }

            [[nodiscard]] bool operator<(const T& o_value) const noexcept(noexcept(m_value < o_value)) {
                return (m_value < o_value);
            }

            [[nodiscard]] bool operator>(const T& o_value) const noexcept(noexcept(m_value > o_value)) {
                return (m_value > o_value );
            }

            [[nodiscard]] bool operator==(const T& o_value) const noexcept(noexcept(m_value == o_value)) {
                return (m_isInstalled && m_value == o_value);
            }

            [[nodiscard]] bool operator!=(const T& o_value) const noexcept(noexcept(m_value != o_value)) {
                return (m_value != o_value);
            }

            [[nodiscard]] bool operator<(const WrapperType<T>& second) const noexcept(noexcept(m_value < second.m_value)) {
                return (m_value < second.m_value);
            }

            [[nodiscard]] bool operator>(const WrapperType<T>& second) const noexcept(noexcept(m_value > second.m_value)) {
                return (m_value > second.m_value);
            }

            [[nodiscard]] bool operator==(const WrapperType<T>& second) const noexcept(noexcept(m_value == second.m_value)) {
                return (m_isInstalled && second.m_isInstalled && m_value == second.m_value);
            }

            [[nodiscard]] bool operator!=(const WrapperType<T>& second) const noexcept(noexcept(m_value != second.m_value)) {
                return ((!m_isInstalled &&  second.m_isInstalled)
                            || (m_isInstalled &&  !second.m_isInstalled)
                            || m_value != second.m_value);
            }

            [[nodiscard]] friend  bool operator<(const T& value, const WrapperType<T>& w) noexcept(noexcept(value < w.m_value)) {
                return (value < w.m_value); 
            }

            [[nodiscard]] friend  bool operator>(const T& value, const WrapperType<T>& w) noexcept(noexcept(value > w.m_value)) {
                return (value > w.m_value);
            }

            [[nodiscard]] friend  bool operator==(const T& value, const WrapperType<T>& w) noexcept(noexcept(value == w.m_value)) {
                return (w.m_isInstalled && value == w.m_value);
            }

            [[nodiscard]] friend  bool operator!=(const T& value, const WrapperType<T>& w) noexcept(noexcept(value != w.m_value)) {
                return (value != w.m_value);
            }

            friend std::ostream&    operator<<(std::ostream& os, const WrapperType<T>& wrapper) {
                if (wrapper.m_isInstalled)
                    os << wrapper.m_value;
                else
                    os << "[empty]";
                return os;
            }

            /**
                @brief Additional methods
                @details Getter-Setters functionallity for work with Wrapper states
                 * - reseting state (Wrapper getting default values)
                 * - getters for read state
                 * - setters for modify state
            */

            void    reset() noexcept(noexcept(m_value = T{})) {
                m_value = T{};
                m_isInstalled = false;
            }
             const T& getValue() const noexcept {
                return m_value;
            }
            void setValue(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
                m_value = value;
                m_isInstalled = true;
            }
             bool hasCustomValue() const noexcept {
                return m_isInstalled;
            }
        private:
            /**
                @brief class Wrapper states
                @details value-initialization{} is immediately defined in the declaration
                            to ensure correctness of further use.
            */
            T       m_value{};
            bool    m_isInstalled{};
    };
}