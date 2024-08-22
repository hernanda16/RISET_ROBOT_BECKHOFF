#ifndef ROLE_ATTACKER_HPP
#define ROLE_ATTACKER_HPP

#include "master/motion.hpp"
#include "master/roles/abstract.hpp"

class Attacker : public RoleAbstract
{
public:
    void gameGeneralHomeSingle() const override;
    void preparationHomeSingle() const override;
    void gameGeneralAwaySingle() const override;
    void preparationAwaySingle() const override;
    void gameGeneralHomeDouble() const override;
    void preparationHomeDouble() const override;
    void gameGeneralAwayDouble() const override;
    void preparationAwayDouble() const override;

    void preparationGlobal() const override;
    void gameStyleA() const override;
    void gameStyleB() const override;
    void gameStyleC() const override;
    void gameStyleD() const override;
    void gameStyleE() const override;

    //=--------Prep Home--------=//
    void PrepKickoffHomeSingle() const override;
    void PrepKickoffHomeDouble() const override;
    void PrepKickoffHomeTriple() const override;
    void PrepKickoffHomeQuadruple() const override;

    void PrepFreekickHomeSingle() const override;
    void PrepFreekickHomeDouble() const override;
    void PrepFreekickHomeTriple() const override;
    void PrepFreekickHomeQuadruple() const override;

    void PrepGoalkickHomeSingle() const override;
    void PrepGoalkickHomeDouble() const override;
    void PrepGoalkickHomeTriple() const override;
    void PrepGoalkickHomeQuadruple() const override;

    void PrepCornerHomeSingle() const override;
    void PrepCornerHomeDouble() const override;
    void PrepCornerHomeTriple() const override;
    void PrepCornerHomeQuadruple() const override;

    void PrepThrowinHomeSingle() const override;
    void PrepThrowinHomeDouble() const override;
    void PrepThrowinHomeTriple() const override;
    void PrepThrowinHomeQuadruple() const override;

    void PrepPenaltyHomeSingle() const override;
    void PrepPenaltyHomeDouble() const override;
    void PrepPenaltyHomeTriple() const override;
    void PrepPenaltyHomeQuadruple() const override;

    //=--------Prep Away--------=//
    void PrepKickoffAwaySingle() const override;
    void PrepKickoffAwayDouble() const override;
    void PrepKickoffAwayTriple() const override;
    void PrepKickoffAwayQuadruple() const override;

    void PrepFreekickAwaySingle() const override;
    void PrepFreekickAwayDouble() const override;
    void PrepFreekickAwayTriple() const override;
    void PrepFreekickAwayQuadruple() const override;

    void PrepGoalkickAwaySingle() const override;
    void PrepGoalkickAwayDouble() const override;
    void PrepGoalkickAwayTriple() const override;
    void PrepGoalkickAwayQuadruple() const override;

    void PrepCornerAwaySingle() const override;
    void PrepCornerAwayDouble() const override;
    void PrepCornerAwayTriple() const override;
    void PrepCornerAwayQuadruple() const override;

    void PrepThrowinAwaySingle() const override;
    void PrepThrowinAwayDouble() const override;
    void PrepThrowinAwayTriple() const override;
    void PrepThrowinAwayQuadruple() const override;

    void PrepPenaltyAwaySingle() const override;
    void PrepPenaltyAwayDouble() const override;
    void PrepPenaltyAwayTriple() const override;
    void PrepPenaltyAwayQuadruple() const override;

    void PrepDropBallSingle() const override;
    void PrepDropBallDouble() const override;
    void PrepDropBallTriple() const override;
    void PrepDropBallQuadruple() const override;

    void PrepparationGlobalDynamic() const override;

    //=--------Main--------=//
    void GameMainSingle() const override;
    void GameMainDouble() const override;
    void GameMainTriple() const override;
    void GameMainQuadruple() const override;

    void HasBall() const override;
    void FriendHasBall() const override;
    void EnemyHasBall() const override;
    void WildBall() const override;
    void Roaming(int default_state = 0) const override;
    void EnemyLock(float enemy_x, float enemy_y) const override;
    void Penalty() const override;
    void Park() const override;
    void BlockEnemy() const override;
    void StandBy(int default_r, float x_arching) const override;
    void Intercept() const override;
};

#endif