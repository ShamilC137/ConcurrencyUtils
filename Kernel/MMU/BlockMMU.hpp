#ifndef APPLICATION_KERNEL_MMU_BLOCKMMU_HPP_
#define APPLICATION_KERNEL_MMU_BLOCKMMU_HPP_

#include "Usings.hpp"

#include <cassert>

namespace mmu {

template<SizeType kBlockSize, SizeType kBlocksNum>
class BlockMMU {
public:
    BlockMMU(VoidPtr ptr) : ptr_{ static_cast<ByteType(*)[kBlockSize]>(ptr) }, is_occupied_vec_{} {}

    [[nodiscard]] VoidPtr Allocate(SizeType n_bytes) noexcept {
        assert(n_bytes <= kBlockSize * kBlockSize && "Lack of a memory for allocation");

        auto req_blocks_num{ n_bytes / kBlockSize };
        if (n_bytes % kBlockSize != 0) {
            ++req_blocks_num;
        }

        auto cycle_last_block{ kBlocksNum - req_blocks_num + 1 };

        for (SizeType block_i{}; block_i < cycle_last_block; ++block_i) {
            if (is_occupied_vec_[block_i]) {
                continue;
            }

            SizeType block_counter{ 1 };
            ++block_i;

            while (block_counter < req_blocks_num) {
                if (is_occupied_vec_[block_i]) {
                    break;
                }
                ++block_counter;
                ++block_i;
            }

            if (block_counter == req_blocks_num) {
                for (SizeType index{ 1 }; index <= req_blocks_num; ++index) {
                    is_occupied_vec_[block_i - index] = true;
                }

                return ptr_ + block_i - req_blocks_num;
            }
        }

        return nullptr;
    }

    void Deallocate(VoidPtr ptr, SizeType n_bytes) noexcept {
        assert(ptr_ <= ptr && ptr_ + kBlocksNum > ptr && "The pointer doesn't belong to this BlockMMU");

        auto blocks_num{ n_bytes / kBlockSize };
        if (n_bytes % kBlockSize != 0) {
            ++blocks_num;
        }

        auto block_i{ static_cast<ByteType(*)[kBlockSize]>(ptr) - ptr_ };
        auto last_block{ block_i + static_cast<PtrDif>(blocks_num) };

        for (; block_i < last_block; ++block_i) {
            is_occupied_vec_[block_i] = false;
        }
    }

private:
    ByteType(*ptr_)[kBlockSize];
    bool is_occupied_vec_[kBlocksNum];
};

}

#endif // APPLICATION_KERNEL_MMU_BLOCKMMU_HPP_
