#include "master/roles/goalkeeper.hpp"

void Goalkeeper::gameGeneralHomeSingle() const
{
    printf("Goalkeeper::gameGeneralHomeSingle() %d\n", game_status->value);
}

void Goalkeeper::preparationAwaySingle() const
{
    printf("Goalkeeper::preparationAwaySingle() %d\n", game_status->value);
}

void Goalkeeper::gameGeneralAwaySingle() const
{
    printf("Goalkeeper::gameGeneralAwaySingle() %d\n", game_status->value);
}

void Goalkeeper::preparationHomeSingle() const
{
    printf("Goalkeeper::preparationHomeSingle() %d\n", game_status->value);
}

void Goalkeeper::gameGeneralHomeDouble() const
{
    printf("Goalkeeper::gameGeneralHomeDouble() %d\n", game_status->value);
}

void Goalkeeper::preparationHomeDouble() const
{
    printf("Goalkeeper::preparationHomeDouble() %d\n", game_status->value);
}

void Goalkeeper::gameGeneralAwayDouble() const
{
    printf("Goalkeeper::gameGeneralAwayDouble() %d\n", game_status->value);
}

void Goalkeeper::preparationAwayDouble() const
{
    printf("Goalkeeper::preparationAwayDouble() %d\n", game_status->value);
}

//=--------Prep Home--------=//
void Goalkeeper::PrepKickoffHomeSingle() const {}
void Goalkeeper::PrepKickoffHomeDouble() const {}
void Goalkeeper::PrepKickoffHomeTriple() const {}
void Goalkeeper::PrepKickoffHomeQuadruple() const {}

void Goalkeeper::PrepFreekickHomeSingle() const {}
void Goalkeeper::PrepFreekickHomeDouble() const {}
void Goalkeeper::PrepFreekickHomeTriple() const {}
void Goalkeeper::PrepFreekickHomeQuadruple() const {}

void Goalkeeper::PrepGoalkickHomeSingle() const {}
void Goalkeeper::PrepGoalkickHomeDouble() const {}
void Goalkeeper::PrepGoalkickHomeTriple() const {}
void Goalkeeper::PrepGoalkickHomeQuadruple() const {}

void Goalkeeper::PrepCornerHomeSingle() const {}
void Goalkeeper::PrepCornerHomeDouble() const {}
void Goalkeeper::PrepCornerHomeTriple() const {}
void Goalkeeper::PrepCornerHomeQuadruple() const {}

void Goalkeeper::PrepThrowinHomeSingle() const {}
void Goalkeeper::PrepThrowinHomeDouble() const {}
void Goalkeeper::PrepThrowinHomeTriple() const {}
void Goalkeeper::PrepThrowinHomeQuadruple() const {}

void Goalkeeper::PrepPenaltyHomeSingle() const {}
void Goalkeeper::PrepPenaltyHomeDouble() const {}
void Goalkeeper::PrepPenaltyHomeTriple() const {}
void Goalkeeper::PrepPenaltyHomeQuadruple() const {}

//=--------Prep Away--------=//
void Goalkeeper::PrepKickoffAwaySingle() const {}
void Goalkeeper::PrepKickoffAwayDouble() const {}
void Goalkeeper::PrepKickoffAwayTriple() const {}
void Goalkeeper::PrepKickoffAwayQuadruple() const {}

void Goalkeeper::PrepFreekickAwaySingle() const {}
void Goalkeeper::PrepFreekickAwayDouble() const {}
void Goalkeeper::PrepFreekickAwayTriple() const {}
void Goalkeeper::PrepFreekickAwayQuadruple() const {}

void Goalkeeper::PrepGoalkickAwaySingle() const {}
void Goalkeeper::PrepGoalkickAwayDouble() const {}
void Goalkeeper::PrepGoalkickAwayTriple() const {}
void Goalkeeper::PrepGoalkickAwayQuadruple() const {}

void Goalkeeper::PrepCornerAwaySingle() const {}
void Goalkeeper::PrepCornerAwayDouble() const {}
void Goalkeeper::PrepCornerAwayTriple() const {}
void Goalkeeper::PrepCornerAwayQuadruple() const {}

void Goalkeeper::PrepThrowinAwaySingle() const {}
void Goalkeeper::PrepThrowinAwayDouble() const {}
void Goalkeeper::PrepThrowinAwayTriple() const {}
void Goalkeeper::PrepThrowinAwayQuadruple() const {}

void Goalkeeper::PrepPenaltyAwaySingle() const {}
void Goalkeeper::PrepPenaltyAwayDouble() const {}
void Goalkeeper::PrepPenaltyAwayTriple() const {}
void Goalkeeper::PrepPenaltyAwayQuadruple() const {}

void Goalkeeper::PrepDropBallSingle() const {}
void Goalkeeper::PrepDropBallDouble() const {}
void Goalkeeper::PrepDropBallTriple() const {}
void Goalkeeper::PrepDropBallQuadruple() const {}

//=--------Main--------=//
void Goalkeeper::GameMainSingle() const {}
void Goalkeeper::GameMainDouble() const {}
void Goalkeeper::GameMainTriple() const {}
void Goalkeeper::GameMainQuadruple() const {}

void Goalkeeper::HasBall() const {}
void Goalkeeper::FriendHasBall() const {}
void Goalkeeper::EnemyHasBall() const {}
void Goalkeeper::WildBall() const {}
void Goalkeeper::Roaming(int default_state) const {}
void Goalkeeper::EnemyLock(float enemy_x, float enemy_y) const {}
void Goalkeeper::Penalty() const {}
void Goalkeeper::Park() const {}

void Goalkeeper::BlockEnemy() const
{
    static obstacle_checking_t obstacle;
    obstacle = motion->obstacleChecking(-90, 180, ball->distance);

    float jarak_obstacle_ke_bola = pythagoras(obstacle.pos_x, obstacle.pos_y, ball->pos.x, ball->pos.y);

    if (ball->pos.y < FIELD_Y_1_4)
    {
        if (obstacle.status && jarak_obstacle_ke_bola > 100 &&
            obstacle.pos_y > FIELD_Y_1_5 &&
            obstacle.pos_y < FIELD_Y_1_2 + 150 &&
            obstacle.pos_y > ball->pos.y)
        {
            motion->motionAroundPointAngle(obstacle.pos_x, obstacle.pos_y,
                                           atan2f(obstacle.pos_y - ball->pos.y, obstacle.pos_x - ball->pos.x) RAD2DEG,
                                           atan2f(obstacle.pos_y - ball->pos.y, obstacle.pos_x - ball->pos.x) RAD2DEG,
                                           50,
                                           80, 60, INVERT);
        }
        else
        {
            motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2,
                                           atan2f(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG,
                                           atan2f(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG,
                                           150,
                                           80, 60, NORMAL_OBSTACLE | INVERT);
        }
    }
    else
    {
        motion->catchingBall(160, 900);
    }
}

void Goalkeeper::StandBy(int default_r, float x_arching) const {}
void Goalkeeper::Intercept() const {}

void Goalkeeper::PrepparationGlobalDynamic() const {}

void Goalkeeper::preparationGlobal() const
{
    printf("Goalkeeper::preparationGlobal() %d\n", game_status->value);
}

void Goalkeeper::gameStyleA() const
{
    printf("Goalkeeper::gameStyleA() %d\n", game_status->value);
}

void Goalkeeper::gameStyleB() const
{
    printf("Goalkeeper::gameStyleB() %d\n", game_status->value);
}

void Goalkeeper::gameStyleC() const
{
    printf("Goalkeeper::gameStyleC() %d\n", game_status->value);
}

void Goalkeeper::gameStyleD() const
{
    printf("Goalkeeper::gameStyleD() %d\n", game_status->value);
}

void Goalkeeper::gameStyleE() const
{
    printf("Goalkeeper::gameStyleE() %d\n", game_status->value);
}
