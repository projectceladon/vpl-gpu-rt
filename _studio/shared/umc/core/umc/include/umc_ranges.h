// Copyright (c) 2008-2019 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __UMC_RANGES_H_
#define __UMC_RANGES_H_

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <ostream>
#include <vector>

namespace UMC {

// Ranges allows holding an ordered list of ranges of [start,end) intervals.
// The canonical example use-case is holding the list of ranges of buffered
// bytes or times in a <video> tag.
template <class T>  // Endpoint type; typically a base::TimeDelta or an int64_t.
class Ranges {
 public:
  // Allow copy & assign.

  // Add (start,end) to this object, coalescing overlaps as appropriate.
  // Returns the number of stored ranges, post coalescing.
  size_t Add(T start, T end);

  // Return the number of disjoint ranges.
  size_t size() const;

  // Return the "i"'th range's start & end (0-based).
  T start(size_t i) const;
  T end(size_t i) const;

  // Return the last range.
  const std::pair<T, T>& back() const;

  // check to see that `entry` is within [start, end) for the given range.
  bool contains(size_t i, const T& entry) const;

  // Shorthand for size() == 0.
  bool empty() const;

  // Clear all ranges.
  void clear();

  // Computes the intersection between this range and |other|.
  Ranges<T> IntersectionWith(const Ranges<T>& other) const;

 private:
  // Disjoint, in increasing order of start.
  std::vector<std::pair<T, T> > ranges_;
};

template<class T>
size_t Ranges<T>::Add(T start, T end) {
  if (start == end)  // Nothing to be done with empty ranges.
    return ranges_.size();

  size_t i;
  // Walk along the array of ranges until |start| is no longer larger than the
  // current interval's end.
  for (i = 0; i < ranges_.size() && ranges_[i].second < start; ++i) {
    // Empty body
  }

  // Now we know |start| belongs in the i'th slot.
  // If i is the end of the range, append new range and done.
  if (i == ranges_.size()) {
    ranges_.push_back(std::make_pair(start, end));
    return ranges_.size();
  }

  // If |end| is less than i->first, then [start,end) is a new (non-overlapping)
  // i'th entry pushing everyone else back, and done.
  if (end < ranges_[i].first) {
    ranges_.insert(ranges_.begin() + i, std::make_pair(start, end));
    return ranges_.size();
  }

  // Easy cases done.  Getting here means there is overlap between [start,end)
  // and the existing ranges.

  // Now: start <= i->second && i->first <= end
  if (start < ranges_[i].first)
    ranges_[i].first = start;
  if (ranges_[i].second < end)
    ranges_[i].second = end;

  // Now: [start,end) is contained in the i'th range, and we'd be done, except
  // for the fact that the newly-extended i'th range might now overlap
  // subsequent ranges.  Merge until discontinuities appear.  Note that there's
  // no need to test/merge previous ranges, since needing that would mean the
  // original loop went too far.
  while ((i + 1) < ranges_.size() &&
         ranges_[i + 1].first <= ranges_[i].second) {
    ranges_[i].second = std::max(ranges_[i].second, ranges_[i + 1].second);
    ranges_.erase(ranges_.begin() + i + 1);
  }

  return ranges_.size();
}

template<class T>
size_t Ranges<T>::size() const {
  return ranges_.size();
}

template<class T>
T Ranges<T>::start(size_t i) const {
  return ranges_[i].first;
}

template<class T>
T Ranges<T>::end(size_t i) const {
  return ranges_[i].second;
}

template <class T>
const std::pair<T, T>& Ranges<T>::back() const {
  return ranges_[size() - 1];
}

template <class T>
bool Ranges<T>::contains(size_t i, const T& entry) const {
  return start(i) <= entry && end(i) > entry;
}

template <class T>
bool Ranges<T>::empty() const {
  return size() == 0;
}

template<class T>
void Ranges<T>::clear() {
  ranges_.clear();
}

template<class T>
Ranges<T> Ranges<T>::IntersectionWith(const Ranges<T>& other) const {
  Ranges<T> result;

  size_t i = 0;
  size_t j = 0;

  while (i < size() && j < other.size()) {
    T max_start = std::max(start(i), other.start(j));
    T min_end = std::min(end(i), other.end(j));

    // Add an intersection range to the result if the ranges overlap.
    if (max_start < min_end)
      result.Add(max_start, min_end);

    if (end(i) < other.end(j))
      ++i;
    else
      ++j;
  }

  return result;
}

} // namespace UMC

#endif  // __UMC_RANGES_H_
