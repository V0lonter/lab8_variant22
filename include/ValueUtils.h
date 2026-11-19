#pragma once

#include <type_traits>

// Aggregate functions of sqlite_orm may return a number, std::optional or
// std::unique_ptr (NULL for an empty table); this helper converts all of them to double.
template <class T>
double asDouble(const T& value)
{
    if constexpr (std::is_arithmetic_v<T>)
    {
        return static_cast<double>(value);
    }
    else if constexpr (requires { value.has_value(); })
    {
        return value.has_value() ? static_cast<double>(*value) : 0.0;
    }
    else
    {
        return value ? static_cast<double>(*value) : 0.0;
    }
}
