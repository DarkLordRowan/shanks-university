#ifndef PROFILING_LIKE_HPP
#define PROFILING_LIKE_HPP
#pragma once

template <typename T>
struct is_profiling : public std::false_type {};

#ifdef SHANKS_ENABLE_PROFILING
template <typename T>
struct is_profiling<shanks::profiling::OperationCounting<T>> : public std::true_type {};
#endif

template <typename T>
concept isProfile = is_profiling<T>::value;

#endif