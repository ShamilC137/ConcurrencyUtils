#ifndef APPLICATION_KERNEL_MMU_ALIGNMMU_HPP_
#define APPLICATION_KERNEL_MMU_ALIGNMMU_HPP_

#include "AllocatorBlockMMU.hpp"

namespace mmu {
template<SizeType kMemorySize>
class AlignMMU {
/**********************************************************************************************************************/
// MEMORY STRUCTURE
// 
//      Memory consists of the two main blocks like the data block and the list  block. The first contains all user's
//      allocations, the list block is a reserved place for the list storing gaps in the first block. Sizes of 
//      the blocks can be determined with the 'kListSize' constant which is the percent of the total memory size that
//      will be occupied by the list. It is evident that the variable is a positive integer value from 0 to 100; but
//      we consider that 5-10% is the most appropriate. 
//     
// MEMORY MANAGEMENT STRATEGY
//
//      The strategy can be devided into steps. If the step cannnot be perfomed, then go to the next one. (all the gaps
//      mentioned below have size equaling to required number of bytes to allocate or more)
//      
//      1.  Find the gap having the same size as required to allocate
//      2.  Find all the gaps whose size is more than 'kBigGapBound' and take the biggest one which has an offset
//          equaling to zero otherwise choose the gap that has  the nearest offset to 'kCommonOffset' 
//      3.  Find the biggest small gap whose offset equals to zero. If it is not presented, choose the gap with 
//          the offset that is the most nearest to 'kCommonOffset'    
//      4.  Throw std::bad_alloc
// 
// GAP LIST
//      
//      The gap list stores empty segments in descending order of their sizes. The empty segment represents EmptyGap 
//      struct:
//      
//          struct EmptyGap {
//              ByteType* beg;
//              ByteType* end;
//          };
// 
//      If there is no place to insert a new gap into the list, std::bad_alloc will be thrown. 
//      
/**********************************************************************************************************************/

private:
    struct EmptyGap {
        ByteType* beg;
        ByteType* end;
    };

public:
    static constexpr SizeType kListSize     = 10u;
    static constexpr SizeType kCommonOffset = 8u;
    static constexpr SizeType kBigGapBound  = 1024u;

    const SizeType kOccupiedByList = kMemorySize * kListSize / 100u / sizeof(ListNode) * sizeof(ListNode);
    const SizeType kListOffset = kMemorySize - kOccupiedByList;

    using ListNode = std::_List_node<EmptyGap, VoidPtr>;
    using Alloc = AllocatorBlockMMU<EmptyGap, sizeof(ListNode), kMemorySize * kListSize / 100u / sizeof(ListNode)>;
    using ListIter = typename ListType<EmptyGap, Alloc>::const_iterator;
    using BlockMMU = BlockMMU<sizeof(ListNode), kMemorySize * kListSize / 100u / sizeof(ListNode)>;

    AlignMMU(VoidPtr ptr) : ptr_{ static_cast<ByteType*>(ptr) },
        block_mmu_{ static_cast<VoidPtr>(ptr_ + kListOffset) },
        alloc_{ &block_mmu_ },
        gap_list_{ alloc_ } {
        static_assert(kMemorySize* kListSize / 100u > sizeof(EmptyGap), "Lack of memory");
        static_assert(!((kMemorySize - 1)& kMemorySize), "Size must be a power of 2");

        gap_list_.push_back({ ptr_, ptr_ + kListOffset });
    }

    
    [[nodiscard]] VoidPtr Allocate(SizeType align, SizeType size) /* throws std::bad_alloc */ {
        assert(!((align - 1) & align) && "Align must be a power of 2");
        assert(size % align == 0 && "Size must be devided by align without a reminder");
        auto end_iter{ gap_list_.end() };

        auto size_bound_iter{ FindSizeBound(size) };
        if (size_bound_iter == gap_list_.begin()) {
            throw std::bad_alloc{};
        }

        auto iter{ FindGapWithSameSize(align, size, size_bound_iter) };
        if (iter != end_iter) {
            return AllocateMemory(align, size, iter);
        }

        auto big_gap_bound_iter = FindBigGapBound();
        bool is_smaller_than_big_gap = true;

        if (big_gap_bound_iter != gap_list_.begin()) {
            ListIter higher_bound;

            if (IsHigherBoundThan(big_gap_bound_iter, size_bound_iter)) {
                higher_bound = big_gap_bound_iter;
            }
            else {
                higher_bound = size_bound_iter;
                is_smaller_than_big_gap = false;
            }

            iter = FindBigGapWithNoOffset(align, higher_bound);
            if (iter != end_iter) {
                return AllocateMemory(align, size, iter);
            }

            iter = FindBigGapWithCommonOffset(align, size, higher_bound);
            if (iter != end_iter) {
                return AllocateMemory(align, size, iter);
            }
        }

        if (is_smaller_than_big_gap) {
            iter = FindSmallGapWithNoOffset(align, big_gap_bound_iter, size_bound_iter);
            if (iter != end_iter) {
                return AllocateMemory(align, size, iter);
            }

            iter = FindSmallGapWithCommonOffset(align, size, big_gap_bound_iter, size_bound_iter);
            if (iter != end_iter) {
                return AllocateMemory(align, size, iter);
            }
        }

        throw std::bad_alloc{};
    }


    void Deallocate(VoidPtr ptr, SizeType size) noexcept {
        auto *ptr_beg{ static_cast<ByteType*>(ptr) };
        auto *ptr_end{ ptr_beg + size };

        auto iter{ gap_list_.begin() };
        auto end_iter{ gap_list_.end() };

        auto *new_beg{ ptr_beg };
        auto *new_end{ ptr_end };

        while(iter != end_iter) {
            auto dif{ ptr_beg - iter->end };
            auto iter_size{ iter->end - iter->beg };

            if (dif >= 0) {
                if (dif >= iter_size) {
                    if (new_beg - iter->beg > 0) {
                        new_beg = iter->beg;
                    }
                    if (new_end - iter->end < 0) {
                        new_end = iter->end;
                    }
                    iter = gap_list_.erase(iter);
                }
            }
            else if (size + dif >= 0) {
                if (new_end - iter->end < 0) {
                    new_end = iter->end;
                }
                iter = gap_list_.erase(iter);
            }
            else {
                ++iter;
            }
        }

        InsertGap({ new_beg, new_end });
    }

private:
    [[nodiscard]] ListIter FindSizeBound(SizeType size) const noexcept {
        auto iter{ gap_list_.begin() };
        auto end_iter{ gap_list_.end() };

        while (iter != end_iter) {
            if (iter->end - iter->beg < static_cast<PtrDif>(size)) return iter;
            ++iter;
        }

        return end_iter;
    }

    [[nodiscard]] ListIter FindGapWithSameSize(SizeType align, SizeType size, ListIter size_bound_iter) const noexcept {
        auto iter{ gap_list_.begin() };

        while (iter != size_bound_iter) {
            if (iter->end - iter->beg == size && !OffsetValue(align, iter)) return iter;
            ++iter;
        }

        return gap_list_.end();
    }

    [[nodiscard]] ListIter FindBigGapBound() const noexcept {
        auto iter{ gap_list_.begin() };
        auto end_iter{ gap_list_.end() };

        while (iter != end_iter) {
            if (iter->end - iter->beg < kBigGapBound) return iter;
            ++iter;
        }

        return end_iter;
    }

    [[nodiscard]] bool IsHigherBoundThan(ListIter higher, ListIter than) const noexcept {
        auto end_iter{ gap_list_.end() };

        if (higher == end_iter) return false;
        else if (than == end_iter) {
            if (higher == end_iter) return false;
            return true;
        }

        if (higher->end - higher->beg > than->end - than->beg) return true;
        return false;
    }

    [[nodiscard]] ListIter FindBigGapWithNoOffset(SizeType align, ListIter bound_iter) const noexcept {
        auto iter{ gap_list_.begin() };

        while (iter != bound_iter) {
            if (!OffsetValue(align, iter)) return iter;
            ++iter;
        }

        return gap_list_.end();
    }

    [[nodiscard]] ListIter FindBigGapWithCommonOffset(SizeType align, SizeType size, 
                                                      ListIter bound_iter) const noexcept {
        auto iter{ gap_list_.begin() };
        ListIter best_iter;
        SizeType min_difference;

        while (iter != bound_iter) {
            auto offset{ OffsetValue(align, iter) };
            if (FitIntoGap(size, iter, offset)) {
                min_difference = DiffernceBetween(offset, kCommonOffset);
                best_iter = iter;
                break;
            }

            ++iter;
        }

        if (iter == bound_iter) return gap_list_.end();
        ++iter;

        while (iter != bound_iter) {
            auto offset = OffsetValue(align, iter);
            auto cur_difference = DiffernceBetween(offset, kCommonOffset);
            if (FitIntoGap(size, iter, offset) && cur_difference < min_difference) {
                min_difference = cur_difference;
                best_iter = iter;
            }
            ++iter;
        }

        return best_iter;
    }

    [[nodiscard]] ListIter FindSmallGapWithNoOffset(SizeType align, ListIter big_gap_bound_iter, 
                                                    ListIter size_bound_iter) const noexcept {
        while (big_gap_bound_iter != size_bound_iter) {
            if (!OffsetValue(align, big_gap_bound_iter)) return big_gap_bound_iter;
        }

        return gap_list_.end();
    }

    [[nodiscard]] ListIter FindSmallGapWithCommonOffset(SizeType align,  SizeType size, ListIter big_gap_bound_iter, 
                                                        ListIter size_bound_iter) const noexcept {
        ListIter best_iter;
        SizeType min_difference;

        while (big_gap_bound_iter != size_bound_iter) {
            auto offset{ OffsetValue(align, big_gap_bound_iter) };
            if (FitIntoGap(size, big_gap_bound_iter, offset)) {
                min_difference = DiffernceBetween(offset, kCommonOffset);
                best_iter = big_gap_bound_iter;
                break;
            }

            ++big_gap_bound_iter;
        }

        if (big_gap_bound_iter == size_bound_iter) return gap_list_.end();
        ++big_gap_bound_iter;

        while (big_gap_bound_iter != size_bound_iter) {
            auto offset{ OffsetValue(align, big_gap_bound_iter) };
            auto cur_difference{ DiffernceBetween(offset, kCommonOffset) };
            if (FitIntoGap(size, big_gap_bound_iter, offset) && cur_difference < min_difference) {
                min_difference = cur_difference;
                best_iter = big_gap_bound_iter;
            }
            ++big_gap_bound_iter;
        }

        return best_iter;
    }

    [[nodiscard]] inline bool FitIntoGap(SizeType size, ListIter gap, SizeType offset) const noexcept {
        return gap->end - (gap->beg + offset) >= static_cast<PtrDif>(size);
    }

    [[nodiscard]] inline SizeType DiffernceBetween(SizeType first, SizeType second) const noexcept {
        return first > second ? first - second : second - first;
    }

    [[nodiscard]] SizeType OffsetValue(SizeType align, ListIter gap) const noexcept{
        if (auto big_offset{ gap->beg - ptr_ }; big_offset % align == 0)
            return 0;
        else return (big_offset / align + 1) * align - big_offset;
    }

    [[nodiscard]] VoidPtr AllocateMemory(SizeType align, SizeType size, ListIter gap) noexcept {
        auto gap_beg{ gap->beg };
        auto gap_end{ gap->end };
        auto offset{ OffsetValue(align, gap) };
        gap_list_.erase(gap);

        if (offset) {
            auto ptr{ gap_beg + offset };

            InsertGap({ gap_beg, ptr });
            InsertGap({ ptr + size, gap_end });

            return ptr;
        }

        InsertGap({ gap_beg + size, gap_end });

        return gap_beg;
    }

    void InsertGap(EmptyGap gap) noexcept {
        auto size{ gap.end - gap.beg };
        auto iter{ gap_list_.begin() };
        auto end_iter{ gap_list_.end() };

        for (; iter != end_iter && iter->end - iter->beg > size; ++iter);

        gap_list_.insert(iter, gap);
    }

    ByteType* ptr_;
    BlockMMU block_mmu_;
    Alloc alloc_;
    ListType<EmptyGap, Alloc> gap_list_;
};

}

#endif // APPLICATION_KERNEL_MMU_ALIGNMMU_HPP_
