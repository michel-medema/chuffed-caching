#ifndef CHUFFED_EVICTIONPOLICY_H
#define CHUFFED_EVICTIONPOLICY_H

class EvictionPolicy {
  public:
    virtual void inserted( int id ) = 0;

    virtual void cacheHit( int id ) = 0;

    virtual void erased( int id ) = 0;

    virtual int getCandidate() = 0;

    virtual ~EvictionPolicy() = default;

};

#endif //CHUFFED_EVICTIONPOLICY_H
