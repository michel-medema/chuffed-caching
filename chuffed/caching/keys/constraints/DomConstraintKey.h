#ifndef CHUFFED_DOMCONSTRAINTKEY_H
#define CHUFFED_DOMCONSTRAINTKEY_H

#include <iostream>
#include <typeinfo>

class DomConstraintKey {
  public:
    const int id;

    explicit DomConstraintKey( int id ) : id( id ) {}

    bool dominates( const DomConstraintKey &c ) const {
      if ( id != c.id ) {
        std::cerr << "Comparing constraints with different IDs." << std::endl;
        return false;
      }

      if ( typeid( *this ) != typeid( c ) ) return false;

      return this->checkDominance( c );
    }

    virtual size_t size() const = 0;

    virtual void print() const {}

    virtual ~DomConstraintKey() = default;

  protected:
    virtual bool checkDominance( const DomConstraintKey &c ) const = 0;
};

#endif //CHUFFED_DOMCONSTRAINTKEY_H
