#pragma once

template <typename Series>
struct SeriesPointerAdapter
{
    const Series *p{};
    const Series *operator->() const { return p; }
};
