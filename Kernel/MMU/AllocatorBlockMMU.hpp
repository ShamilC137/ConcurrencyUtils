#ifndef APPLICATION_KERNEL_MMU_ALLOCATORBLOCKMMU_HPP_
#define APPLICATION_KERNEL_MMU_ALLOCATORBLOCKMMU_HPP_

#include "BlockMMU.hpp"

namespace mmu {

template<class T, SizeType kBlockSize, SizeType kBlocksNum>
class AllocatorBlockMMU {
public:
    using value_type = T;

    using is_always_equal = std::false_type;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    template<class U>
    struct rebind {
        using other = AllocatorBlockMMU<U, kBlockSize, kBlocksNum>;
    };

    AllocatorBlockMMU(BlockMMU<kBlockSize, kBlocksNum>* mmu) noexcept : mmu_{ mmu } {}

    template<class U>
    AllocatorBlockMMU(const AllocatorBlockMMU<U, kBlockSize, kBlocksNum>& other) noexcept : mmu_{ other.mmu_ } {}

    template<class U>
    AllocatorBlockMMU& operator=(const AllocatorBlockMMU<U, kBlockSize, kBlocksNum>& other) noexcept {
        mmu_ = other.mmu_;
    }

    [[nodiscard]] T* allocate(SizeType num) /*throws: std::bad_alloc*/ {
        auto ptr{ static_cast<T*>(mmu_->Allocate(num * sizeof(T))) };
        if (ptr == nullptr) {
            throw std::bad_alloc{};
        }
        return ptr;
    }

    void deallocate(T* ptr, SizeType num) noexcept {
        mmu_->Deallocate(ptr, num * sizeof(T));
    }

    BlockMMU<kBlockSize, kBlocksNum>* mmu_;
};

template<class T, class U, SizeType kBlockSize, SizeType kBlocksNum>
[[nodiscard]] bool operator==(AllocatorBlockMMU<T, kBlockSize, kBlocksNum> lhs, 
                              AllocatorBlockMMU<U, kBlockSize, kBlocksNum> rhs) noexcept {
    return lhs.mmu_ == rhs.mmu_;
}

template<class T, class U, SizeType kBlockSize, SizeType kBlocksNum>
[[nodiscard]] bool operator!=(AllocatorBlockMMU<T, kBlockSize, kBlocksNum> lhs, 
                              AllocatorBlockMMU<U, kBlockSize, kBlocksNum> rhs) noexcept {
    return !(lhs == rhs);
}

}

#endif // APPLICATION_KERNEL_MMU_ALLOCATORBLOCKMMU_HPP_