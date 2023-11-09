#include "foamlib"
#pile

CommutativeStar: Category == with
noZeroDivisors: Category == with
Magma: Category == with

LeftModule(L: SemiRng): Category == with
RightModule(L: SemiRng): Category == with

BiModule(L: SemiRng, R: SemiRng): Category == with
    LeftModule L
    RightModule R

AbelianSemiGroup: Category == with
    NonAssociativeSemiRng
    AbelianMonoid
    
NonAssociativeSemiRing: Category == with

NonAssociativeSemiRng: Category == with
    
XMonoid: Category == with
SemiGroup: Category == with

SemiRng: Category == with
    NonAssociativeSemiRng
    BiModule(%, %)
    SemiGroup
    
#if 0
SemiRing: Category == Join(NonAssociativeSemiRing, SemiRng, XMonoid) with

Algebra(R: CommutativeRing): Category == Ring with

CommutativeRing: Category == with
    Ring
    Algebra %
    CommutativeStar

EntireRing: Category == with
    Ring
    noZeroDivisors
    
IntegralDomain: Category == with
    CommutativeRing
    EntireRing

Integer: IntegralDomain == never
#endif
