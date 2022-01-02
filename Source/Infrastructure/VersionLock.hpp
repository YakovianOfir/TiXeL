#pragma once

/////////////////////////////////////////////////////////////////
//
//   Copyright (c) Ofir Yakovian. All rights reserved.
//
//   Module Name:
//
//      | VersionLock.hpp |
//
//   Author:
//
//      | Ofir Yakovian (@ofirkov) |
//
//   Environment:
//
//      | User Mode |
//
/////////////////////////////////////////////////////////////////

#include <atomic>
#include <cstdint>
#include "LockBase.hpp"

namespace Tixel::Infra
{
    class VersionLock : public LockBase
    {
        public:
            enum class FlagOption
            {
                Disable = 0,
                Enable = 1
            };

        public:
            explicit VersionLock();

        public:
            bool TryAcquire() override;
            void Release() override;

        public:
            void SetVersion(uint64_t version);
            void SetDeleted(FlagOption option);
            void SetSingleton(FlagOption option);
            uint64_t GetVersion() const;

        public:
            bool IsLocked() const;
            bool IsDeleted() const;
            bool IsSingleton() const;
            bool IsLockedOrDeleted() const;

        public:
            bool ValidVersionAndReleased(uint64_t version) const;

        private:
            uint64_t Load() const;
            void Store(uint64_t value);

        private:
            static uint64_t LockStateMask;
            static uint64_t DeleteStateMask;
            static uint64_t SingletonStateMask;
            static uint64_t VersionNegativeMask;

        private:
            std::atomic<uint64_t> _versionAndFlags;
    };
}