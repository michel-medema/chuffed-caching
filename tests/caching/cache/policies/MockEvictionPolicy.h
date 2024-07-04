#ifndef CHUFFED_MOCKEVICTIONPOLICY_H
#define CHUFFED_MOCKEVICTIONPOLICY_H

#include "gmock/gmock.h"

#include "chuffed/caching/cache/policies/EvictionPolicy.h"

class MockEvictionPolicy: public EvictionPolicy {
  public:
    MOCK_METHOD(void, inserted, (int id), (override));

    MOCK_METHOD(void, cacheHit, (int id), (override));

    MOCK_METHOD(void, erased, (int id), (override));

    MOCK_METHOD(int, getCandidate, (), (override));
};

#endif //CHUFFED_MOCKEVICTIONPOLICY_H
