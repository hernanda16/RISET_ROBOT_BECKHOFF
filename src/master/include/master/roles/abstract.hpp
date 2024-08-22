#ifndef ROLE_ABSTRACTOR_HPP
#define ROLE_ABSTRACTOR_HPP

#include "master/utils.hpp"

#define ROLE_GOALKEEPER 0
#define ROLE_ATTACKER 1
#define ROLE_DEFENDER 2

class RoleAbstract
{
public:
    /**
     * @brief Shared data
     * Jangan lupa melakukan syncMemory() pada master_definition.cpp
     */
    pose2d_t *final_pose;
    ball_t *ball;
    MachineState *game_status;
    bs_t *bs_data;
    pc2stm_t *stm_data;
    Motion *motion;
    int16_t *robot_condition;
    shrobot_data_t *shared_robot_data[4];

    // HASBALL
    float *pos_buffer_robot_has_ball[2];
    uint8_t *hisap_dribble_lama;
    float *titik_final_positioning[2];

    /**
     * @brief Local data
     * Data ini tidak akan di share ke role lain namun bisa diakses dari master
     * @note even number is for prep cycle, odd number is for game cycle
     */
    MachineState *game_sub_status_prep;
    MachineState *game_sub_status;

    uint8_t *has_prep;

    uint8_t *enemy_locked;
    obstacle_checking_t *enemy_locked_obs;
    float *bola_terakhir_sebelum_hilang[2];
    uint8_t *receive_lock;

    float *region_by_style;
    float *prev_ball[2];

    int *prev_n_has_ball;

    uint8_t *lock_goal_kiri_atau_kanan;

    /**
     * @brief Shared method
     * Method ini diakses oleh role lain, bisa diisi terserah role tersebut
     */
    virtual void gameGeneralHomeSingle() const = 0;
    virtual void preparationHomeSingle() const = 0;
    virtual void gameGeneralAwaySingle() const = 0;
    virtual void preparationAwaySingle() const = 0;

    virtual void gameGeneralHomeDouble() const = 0;
    virtual void preparationHomeDouble() const = 0;
    virtual void gameGeneralAwayDouble() const = 0;
    virtual void preparationAwayDouble() const = 0;

    virtual void preparationGlobal() const = 0;
    virtual void gameStyleA() const = 0;
    virtual void gameStyleB() const = 0;
    virtual void gameStyleC() const = 0;
    virtual void gameStyleD() const = 0;
    virtual void gameStyleE() const = 0;

    const pose2d_t BASE_1 = {760, 400.0, 180};
    const pose2d_t BASE_2 = {37.5, 400.0, 0};

    //=--------Prep Home--------=//
    virtual void PrepKickoffHomeSingle() const = 0;
    virtual void PrepKickoffHomeDouble() const = 0;
    virtual void PrepKickoffHomeTriple() const = 0;
    virtual void PrepKickoffHomeQuadruple() const = 0;

    virtual void PrepFreekickHomeSingle() const = 0;
    virtual void PrepFreekickHomeDouble() const = 0;
    virtual void PrepFreekickHomeTriple() const = 0;
    virtual void PrepFreekickHomeQuadruple() const = 0;

    virtual void PrepGoalkickHomeSingle() const = 0;
    virtual void PrepGoalkickHomeDouble() const = 0;
    virtual void PrepGoalkickHomeTriple() const = 0;
    virtual void PrepGoalkickHomeQuadruple() const = 0;

    virtual void PrepCornerHomeSingle() const = 0;
    virtual void PrepCornerHomeDouble() const = 0;
    virtual void PrepCornerHomeTriple() const = 0;
    virtual void PrepCornerHomeQuadruple() const = 0;

    virtual void PrepThrowinHomeSingle() const = 0;
    virtual void PrepThrowinHomeDouble() const = 0;
    virtual void PrepThrowinHomeTriple() const = 0;
    virtual void PrepThrowinHomeQuadruple() const = 0;

    virtual void PrepPenaltyHomeSingle() const = 0;
    virtual void PrepPenaltyHomeDouble() const = 0;
    virtual void PrepPenaltyHomeTriple() const = 0;
    virtual void PrepPenaltyHomeQuadruple() const = 0;

    //=--------Prep Away--------=//
    virtual void PrepKickoffAwaySingle() const = 0;
    virtual void PrepKickoffAwayDouble() const = 0;
    virtual void PrepKickoffAwayTriple() const = 0;
    virtual void PrepKickoffAwayQuadruple() const = 0;

    virtual void PrepFreekickAwaySingle() const = 0;
    virtual void PrepFreekickAwayDouble() const = 0;
    virtual void PrepFreekickAwayTriple() const = 0;
    virtual void PrepFreekickAwayQuadruple() const = 0;

    virtual void PrepGoalkickAwaySingle() const = 0;
    virtual void PrepGoalkickAwayDouble() const = 0;
    virtual void PrepGoalkickAwayTriple() const = 0;
    virtual void PrepGoalkickAwayQuadruple() const = 0;

    virtual void PrepCornerAwaySingle() const = 0;
    virtual void PrepCornerAwayDouble() const = 0;
    virtual void PrepCornerAwayTriple() const = 0;
    virtual void PrepCornerAwayQuadruple() const = 0;

    virtual void PrepThrowinAwaySingle() const = 0;
    virtual void PrepThrowinAwayDouble() const = 0;
    virtual void PrepThrowinAwayTriple() const = 0;
    virtual void PrepThrowinAwayQuadruple() const = 0;

    virtual void PrepPenaltyAwaySingle() const = 0;
    virtual void PrepPenaltyAwayDouble() const = 0;
    virtual void PrepPenaltyAwayTriple() const = 0;
    virtual void PrepPenaltyAwayQuadruple() const = 0;

    virtual void PrepDropBallSingle() const = 0;
    virtual void PrepDropBallDouble() const = 0;
    virtual void PrepDropBallTriple() const = 0;
    virtual void PrepDropBallQuadruple() const = 0;

    virtual void PrepparationGlobalDynamic() const = 0;

    //=--------Main--------=//
    virtual void GameMainSingle() const = 0;
    virtual void GameMainDouble() const = 0;
    virtual void GameMainTriple() const = 0;
    virtual void GameMainQuadruple() const = 0;

    virtual void HasBall() const = 0;
    virtual void FriendHasBall() const = 0;
    virtual void EnemyHasBall() const = 0;
    virtual void WildBall() const = 0;
    virtual void Roaming(int default_state = 0) const = 0;
    virtual void EnemyLock(float enemy_x, float enemy_y) const = 0;
    virtual void Penalty() const = 0;
    virtual void Park() const = 0;
    virtual void BlockEnemy() const = 0;
    virtual void StandBy(int default_r, float x_arching) const = 0;
    virtual void Intercept() const = 0;
};

#endif