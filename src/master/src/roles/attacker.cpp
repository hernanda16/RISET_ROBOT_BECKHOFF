#include "master/roles/attacker.hpp"

void Attacker::preparationAwaySingle() const
{
    printf("Attacker::preparationAwaySingle() %d\n", game_status->value);
}

void Attacker::gameGeneralAwaySingle() const
{
    printf("Attacker::gameGeneralAwaySingle() %d\n", game_status->value);
}

void Attacker::preparationHomeSingle() const
{
    printf("Attacker::preparationHomeSingle() %d\n", game_status->value);
}

void Attacker::gameGeneralHomeSingle() const
{
    printf("Attacker::gameGeneralHomeSingle() %d\n", game_status->value);
}

void Attacker::preparationHomeDouble() const
{
    printf("Attacker::preparationHomeDouble() %d\n", game_status->value);
}

void Attacker::gameGeneralHomeDouble() const
{
    printf("Attacker::gameGeneralHomeDouble() %d\n", game_status->value);
}

void Attacker::gameGeneralAwayDouble() const
{
    printf("Attacker::gameGeneralAwayDouble() %d\n", game_status->value);
}

void Attacker::preparationAwayDouble() const
{
    printf("Attacker::preparationAwayDouble() %d\n", game_status->value);
}

//=--------Prep Home--------=//

//==========================================//
//=--------Preparation Kickoff Home-------=//
//==========================================//
void Attacker::PrepKickoffHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepKickoffHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->ruckigToPoint(FIELD_X_2_3, FIELD_Y_1_3, 90, 100, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        if (ball->distance < 250 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->status > 0) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}

void Attacker::PrepKickoffHomeDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepKickoffHomeDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->ruckigToPoint3(FIELD_X_2_3, FIELD_Y_1_3, 90, 100, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        if (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && (ball->status > 0 || bs_data->n_nearest_ball > 0)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(170, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepKickoffHomeTriple() const { }
void Attacker::PrepKickoffHomeQuadruple() const { }

//==========================================//
//=--------Preparation Freekick Home-------=//
//==========================================//
void Attacker::PrepFreekickHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepFreekickHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, 90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 100;
        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 100 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 100 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance <= 300) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepFreekickHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.04);
    printf("Attacker::PrepFreekickHomeDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    static float angle_target;
    static float target_[2];

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;
        printf("prep freekick home double att\n");
        break;
    case 1:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 120, 70, NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 2;
        }

        break;
    case 2:
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 120, 70, NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 3;
        }
        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 250) {
            game_sub_status_prep->value = 3;
        }

        break;
    case 3:
        float angle_target;
        if (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 70)
            angle_target = 180;
        else
            angle_target = atan2(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x);

        target_[0] = ball->pos.x + 350 * cosf(angle_target);
        target_[1] = ball->pos.y + 350 * sinf(angle_target);
        motion->ruckigToPoint3(target_[0], target_[1], motion->RobotAngletoBall(), 150, 90);
        break;

    default:
        break;
    }
}
void Attacker::PrepFreekickHomeTriple() const { }
void Attacker::PrepFreekickHomeQuadruple() const { }

//==========================================//
//=--------Preparation Goalkick Home-------=//
//==========================================//
void Attacker::PrepGoalkickHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepGoalkickHomeSingle() %d %.2f\n", game_sub_status_prep->value, ball->distance);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 100;
        if (ball->status > 0 && ball->pos.y < FIELD_Y_1_4 && ball->pos.y > FIELD_Y_0 + 50) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
            counter = 0;
        }
        if (ball->distance <= 170) {
            game_sub_status_prep->value = 101;
            counter = 0;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepGoalkickHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.04);
    printf("Attacker::PrepGoalkickHomeDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    printf("prep goalkick home double att || %d %.2f \n", game_sub_status_prep->value, pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y));
    printf("n_nearest ball %d,ball_x %.2f, ball_y %2.f", bs_data->n_nearest_ball, ball->pos.x, ball->pos.y);

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;
        printf("prep goalkick home double att\n");
        break;
    case 1:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, 90, 120, 70, NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 2;
        }
        break;
    case 2:
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 120, 70, NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 3;
        }
        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 90) {
            game_sub_status_prep->value = 3;
        }

        break;
    case 3:
        motion->motionAroundBall(atan2(shared_robot_data[bs_data->n_robot_def_1]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_def_1]->pose.x - ball->pos.x) RAD2DEG, atan2(shared_robot_data[bs_data->n_robot_def_1]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_def_1]->pose.x - ball->pos.x) RAD2DEG, 70, 100, 70);
        break;

    default:
        break;
    }
}
void Attacker::PrepGoalkickHomeTriple() const { }
void Attacker::PrepGoalkickHomeQuadruple() const { }

//========================================//
//=--------Preparation Corner Home-------=//
//========================================//
void Attacker::PrepCornerHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepCornerHomeSingle() %d %d %.2f %.2f\n", game_sub_status_prep->value, ball->status, ball->pos.x, ball->pos.y);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 100;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_4_5, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status_prep->value = 102;
            }
            if (deltaAngle(final_pose->theta, motion->BallAngletoOurGoal() - 180) < 15) {
                game_sub_status_prep->value = 102;
            }
        } else {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status_prep->value = 102;
            }
            if (deltaAngle(final_pose->theta, motion->BallAngletoOurGoal() - 180) < 15) {
                game_sub_status_prep->value = 102;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 60, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 70, 60, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepCornerHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.05);
    printf("Attacker::PrepCornerHomeDouble() %d\n", game_sub_status_prep->value);
    printf("ball_status : %d %d %.2f %.2f\n", ball->status, bs_data->n_nearest_ball, ball->pos.x, ball->pos.y);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;
        printf("prep goalkick home double att\n");
        break;
    case 1:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_2_3, 90, 150, 90, NORMAL_OBSTACLE | normal_obstacle_ball)) {
            game_sub_status_prep->value = 2;
        }

        break;
    case 2:
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 90, NORMAL_OBSTACLE | normal_obstacle_ball)) {
            game_sub_status_prep->value = 3;
        }
        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 350) {
            game_sub_status_prep->value = 3;
        }

        break;
    case 3: {
        float sudut_target = atan2f(FIELD_Y_1_2 + 150 - ball->pos.y, FIELD_X_1_2 - ball->pos.x);
        float target_x = ball->pos.x + 350 * cosf(sudut_target);
        float target_y = ball->pos.y + 350 * sinf(sudut_target);

        motion->ruckigToPoint3(target_x, target_y, motion->RobotAngletoBall(), 155, 90, NORMAL_OBSTACLE);
    }

    default:
        break;
    }
}
void Attacker::PrepCornerHomeTriple() const { }
void Attacker::PrepCornerHomeQuadruple() const { }

//=========================================//
//=--------Preparation Throwin Home-------=//
//=========================================//
void Attacker::PrepThrowinHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepThrowinHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 12;
        } else {
            game_sub_status_prep->value = 13;
        }
        break;

    case 12:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 13;
        }
        break;

    case 13:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 12;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 65, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepThrowinHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.05);
    printf("Attacker::PrepThrowinHomeDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        printf("prep throwin home double att\n");
        break;

    default:
        break;
    }
}
void Attacker::PrepThrowinHomeTriple() const { }
void Attacker::PrepThrowinHomeQuadruple() const { }

//=========================================//
//=--------Preparation Penalty Home-------=//
//=========================================//
void Attacker::PrepPenaltyHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepPenaltyHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_2 - 100, robotAngletoEnemyGoal(*final_pose), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        if (ball->distance < 100 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->status > 0 && final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        motion->motionAroundBall(robotAngletoEnemyGoal(*final_pose), robotAngletoEnemyGoal(*final_pose), 70, 120, 70, NORMAL_OBSTACLE);
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepPenaltyHomeDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepPenaltyHomeDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 10:
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_2 - 150, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 100;
        }
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 70 && final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 100;
        }
        break;

        //===============================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance < 200) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (motion->hadapGawangVisionPivot(ball->pos.x, ball->pos.y, 60, 120, 80)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}
void Attacker::PrepPenaltyHomeTriple() const { }
void Attacker::PrepPenaltyHomeQuadruple() const { }

//=--------Prep Away--------=//

//=========================================//
//=--------Preparation Kickoff Away-------=//
//=========================================//
void Attacker::PrepKickoffAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepKickoffAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->distance < 250) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->distance < 250) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, motion->RobotAngletoPoint(400, 600), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepKickoffAwayDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepKickoffAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->distance < 250) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && ball->distance < 250) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, motion->RobotAngletoPoint(400, 600), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepKickoffAwayTriple() const { }
void Attacker::PrepKickoffAwayQuadruple() const { }

//==========================================//
//=--------Preparation Freekick Away-------=//
//==========================================//
void Attacker::PrepFreekickAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepFreekickAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================
    case 10:
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 400) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 400) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoPoint(400, 600), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance <= 400) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepFreekickAwayDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepFreekickAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================
    case 10:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 400) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 400) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, 90, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance <= 400) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepFreekickAwayTriple() const { }
void Attacker::PrepFreekickAwayQuadruple() const { }

//==========================================//
//=--------Preparation Goalkick Away-------=//
//==========================================//
void Attacker::PrepGoalkickAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepGoalkickAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4 && ball->pos.y > FIELD_Y_0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, motion->RobotAngletoPoint(400, 600), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance <= 400) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepGoalkickAwayDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepGoalkickAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;
        case 'B':
            game_sub_status_prep->value = 10;
            break;
        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        if (ball->distance <= 400) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepGoalkickAwayTriple() const { }
void Attacker::PrepGoalkickAwayQuadruple() const { }

//==========================================//
//=--------Preparation Corner Away---------=//
//==========================================//
void Attacker::PrepCornerAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepCornerAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0 && ball->pos.y < FIELD_Y_1_4) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 && ball->pos.y < FIELD_Y_1_4 && final_pose->y > FIELD_Y_1_4 && final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 101;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        break;

        //==================================================================================================================
    case 101:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0)) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->ruckigToPoint3(FIELD_X_1_4, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
            if (motion->RobotDist2Point(FIELD_X_1_4, FIELD_Y_1_3) < 100) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
        } else {
            if (motion->ruckigToPoint3(FIELD_X_3_4, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
            if (motion->RobotDist2Point(FIELD_X_3_4, FIELD_Y_1_3) < 100) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 100:
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_2, ball->pos.x - FIELD_X_1_4) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_2, ball->pos.x - FIELD_X_3_4) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        break;
    }
}
void Attacker::PrepCornerAwayDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepCornerAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && ball->pos.y < FIELD_Y_1_4) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && ball->pos.y < FIELD_Y_1_4 && final_pose->y > FIELD_Y_1_4 & final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 101;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }
        break;

        //==================================================================================================================
    case 101:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0)) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->ruckigToPoint3(FIELD_X_1_4, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
            if (motion->RobotDist2Point(FIELD_X_1_4, FIELD_Y_1_3) < 100) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
        } else {
            if (motion->ruckigToPoint3(FIELD_X_3_4, FIELD_Y_1_3, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
            if (motion->RobotDist2Point(FIELD_X_3_4, FIELD_Y_1_3) < 100) {
                counter = 0;
                game_sub_status_prep->value = 100;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 100:
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_2, ball->pos.x - FIELD_X_1_4) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_2, ball->pos.x - FIELD_X_3_4) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        break;
    }
}
void Attacker::PrepCornerAwayTriple() const { }
void Attacker::PrepCornerAwayQuadruple() const { }

//=========================================//
//=--------Preparation Throwin Away-------=//
//=========================================//
void Attacker::PrepThrowinAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepThrowinAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        }

        if (final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 12;
        } else {
            game_sub_status_prep->value = 13;
        }
        break;

    case 12:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 13;
        }
        break;

    case 13:
        if (ball->status > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 12;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->distance < 300) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
            if (final_pose->y < 200) {
                game_sub_status_prep->value = 102;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_4, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
            if (ball->pos.y > FIELD_Y_1_3) {
                game_sub_status_prep->value = 101;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepThrowinAwayDouble() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepThrowinAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        } else {
            game_sub_status_prep->value = 11;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (final_pose->y < FIELD_Y_1_2) {
            game_sub_status_prep->value = 12;
        } else {
            game_sub_status_prep->value = 13;
        }
        break;

    case 12:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 13;
        }
        break;

    case 13:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, 0, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 12;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->distance < 300) {
            game_sub_status_prep->value = 101;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
            if (final_pose->y < 200) {
                game_sub_status_prep->value = 102;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_4, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
            if (ball->pos.y > FIELD_Y_1_3) {
                game_sub_status_prep->value = 101;
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    }
}
void Attacker::PrepThrowinAwayTriple() const { }
void Attacker::PrepThrowinAwayQuadruple() const { }

//=========================================//
//=--------Preparation Penalty Away-------=//
//=========================================//
void Attacker::PrepPenaltyAwaySingle() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepPenaltyAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        motion->motionToPoint(FIELD_X_3_4, FIELD_Y_1_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);

        break;

    default:
        break;
    }
}
void Attacker::PrepPenaltyAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepPenaltyAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        motion->motionToPoint(FIELD_X_3_4, FIELD_Y_1_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);

        break;

    default:
        break;
    }
}
void Attacker::PrepPenaltyAwayTriple() const { }
void Attacker::PrepPenaltyAwayQuadruple() const { }

//=========================================//
//=---------Preparation Dropball----------=//
//=========================================//
void Attacker::PrepDropBallSingle() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepDropBallSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        case 'B':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;

    case 100:
        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        break;

    case 101:
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 170, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 105;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 200) {
            game_sub_status_prep->value = 105;
        }
        break;

    case 105:
        motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 120, 170, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        break;
    }
}
void Attacker::PrepDropBallDouble() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Attacker::PrepDropBallDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        case 'B':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;

    case 100:
        if ((ball->status > 0 || bs_data->n_nearest_ball > 0) && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        break;

    case 101:
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 170, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 105;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 200) {
            game_sub_status_prep->value = 105;
        }
        break;
    case 105:
        motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 120, 170, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        break;
    }
}
void Attacker::PrepDropBallTriple() const { }
void Attacker::PrepDropBallQuadruple() const { }

void Attacker::PrepparationGlobalDynamic() const
{
    static float target_x_buffer = 0;
    static float target_y_buffer = 0;

    static const float v_maks = 3;
    static const float batas_pindah_jauh_r = 600;

    static uint8_t counter = 0;

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    stm_data->force_dribble_control = 1;

    game_sub_status_prep->reentry(0, 0.5);
    LogWithColor("white", "Attacker::PrepparationGlobalDynamic() %d %d\n", game_sub_status_prep->value, ++counter);
    switch (game_sub_status_prep->value) {
    case 0:
        if (*titik_final_positioning[0] > FLT_EPSILON && *titik_final_positioning[1] > FLT_EPSILON) {

            {
                float dx = *titik_final_positioning[0] - target_x_buffer;
                float dy = *titik_final_positioning[1] - target_y_buffer;

                float a = atan2(dy, dx);
                float r = sqrtf(dx * dx + dy * dy);

                if (r > v_maks && r < batas_pindah_jauh_r)
                    r = v_maks;

                target_x_buffer += r * cosf(a);
                target_y_buffer += r * sinf(a);
            }

            if (game_status->value - 128 == status_preparation_cornerkick_home) {
                if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4)
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoBall(), 150, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
                else
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 250, 15, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
            } else if (game_status->value - 128 == status_preparation_goalkick_home) {
                if (ball->status > 0 && ball->pos.y < FIELD_Y_1_3)
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoBall(), 150, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
                else
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 250, 15, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
            } else if (game_status->value - 128 == status_preparation_freekick_home) {
                if (ball->status > 0 && ball->pos.y < FIELD_Y_3_4)
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoBall(), 150, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
                else
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 250, 15, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
            } else {
                if (ball->status > 0)
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoBall(), 150, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
                else
                    motion->motionToPoint(target_x_buffer, target_y_buffer, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 250, 15, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
            }
        }
        break;
    }
}

//=--------Main--------=//
void Attacker::GameMainSingle() const
{
    static int prev_n_has_ball;
    static uint64_t pass_timer[2];
    static uint64_t time_update = 0;
    static float ball_buffer[2];
    static float pos_robot_buffer[2];
    static obstacle_checking_t obs;
    static const int ring_rebut_bola_min = 0;
    static const int ring_rebut_bola_max = 300;

    static uint16_t counter_bola_keluar_ring = 0;

    static uint16_t counter_diam_sebentar = 0;

    // OBSTACLE AVOIDANCE MENGHADAP BOLA
    obs = motion->obstacleChecking(motion->RobotAngletoBall(), 30, ball->distance + 50, NORMAL_OBSTACLE);

    game_sub_status->reentry(0, 0.05);
    printf("Attacker::GameMainSingle() %d || %d\n", game_sub_status->value, *has_prep);

    // HAS PREP = 0 -> HOME
    // HAS PREP = 1 -> MASUK KE LAYER 2 ATAU DROP BALL
    // HAS PREP = 2 -> AWAY
    // HAS PREP = 3 -> PENALTY

    //*================================================
    //*                   LAYER 1
    //*================================================

    if (*has_prep == 1)
        game_sub_status->value = 100;

    switch (game_sub_status->value) {
    case 0:
        /**
         * MENENTUKAN STATE
         */
        if (*has_prep == 0) // Home
            game_sub_status->value = 1;
        else if (*has_prep == 2) // Away
            game_sub_status->value = 2;
        else if (*has_prep == 3) // Special
        {
            game_sub_status->value = 3;
        }

        counter_diam_sebentar = 0;

        stm_data->force_dribble_control = 1;

        game_sub_status->resetUptimeTimeout();

        break;
    case 1:
        /**
         *? HOME
         * CATCHING BALL
         * NEXT STATE -> 10
         */
        ball_buffer[0] = ball->pos.x;
        ball_buffer[1] = ball->pos.y;
        game_sub_status->value = 1000;
        motion->manualMotion(0, 0, 0);
        game_sub_status->resetUptimeTimeout();

        break;

    case 1000:
        if (ball->distance > 150) {
            ball_buffer[0] = ball->pos.x;
            ball_buffer[1] = ball->pos.y;
        }
        motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 80, 180, 120, NORMAL_OBSTACLE);

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 50 && ball->distance < 120) {
            counter_bola_keluar_ring++;
        } else {
            counter_bola_keluar_ring = 0;
        }

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 70 && counter_bola_keluar_ring > 4) {
            counter_bola_keluar_ring = 0;
            game_sub_status->value = 99;
        }
        break;

    case 10:
        /**
         *? HOME
         * MENGARAHKAN BOLA
         * NEXT STATE -> 11
         */

        if (ball->is_caught > 0) {
            kickerControl(stm_data, OFFBALLWITHDRIBBLE, 2);
            game_sub_status->value = 11;
        }

        if (game_status->value - 128 == status_preparation_cornerkick_home) {
            if (motion->pivotMotion(final_pose->theta, 180)) {
                game_sub_status->value = 11;
            }
        } else if (game_status->value - 128 == status_preparation_goalkick_home) {
            if (motion->pivotMotion(final_pose->theta, 180)) {
                game_sub_status->value = 11;
            }
        } else if (game_status->value - 128 == status_preparation_freekick_home) {
            if (motion->pivotMotion(robotAngletoEnemyGoal(*final_pose) - 180, 180)) {
                game_sub_status->value = 11;
            }
        } else if (game_status->value - 128 == status_preparation_throwin_home) {
            if (motion->pivotMotion(final_pose->theta, 180)) {
                game_sub_status->value = 11;
            }
        } else {
            if (motion->pivotMotion(-135, 180)) {
                game_sub_status->value = 11;
            }
        }

        if (ball->is_caught == 0) {
            game_sub_status->value = 1;
        }

        break;

    case 11:
        /**
         *? HOME
         * TENDANG BOLA
         * NEXT STATE -> 110
         */
        kickerControl(stm_data, OFFBALLWITHDRIBBLE, 50);
        if (ball->is_caught == 0) {
            game_sub_status->value = 110;
            game_sub_status->resetUptimeTimeout();
        }

        break;

    case 110:
        /**
         *? HOME
         * CATCHING BALL
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */
        motion->catchingBall(190, 120, LANGSUNG_AMBIL);

        if (ball->is_caught == 2)
            game_sub_status->value = 99;

        game_sub_status->timeout(99, 6);

        break;

    case 2:
        /**
         *! AWAY
         * SELALU MENCATAT POSISI
         * RULE RADIUS : KICKOFF > 200 | CORNER THROWIN GOALKICK FREEKICK > 300
         * NEXT STATE -> 20 (PERSIAPAN MASUK LAYER 2)
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */

        // pos_robot_buffer[0] = final_pose->x;
        // pos_robot_buffer[1] = final_pose->y;
        // if (game_status->value - 128 == status_preparation_cornerkick_away) {
        //     if (motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE)) {
        //         game_sub_status->value = 20;
        //         pos_robot_buffer[0] = final_pose->x;
        //         pos_robot_buffer[1] = final_pose->y;
        //     }
        // } else {
        //     if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE)) {
        //         game_sub_status->value = 20;
        //         pos_robot_buffer[0] = final_pose->x;
        //         pos_robot_buffer[1] = final_pose->y;
        //     }
        // }

        // !KELUAR KARENA TIMEOUT 7 DETIK
        // game_sub_status->timeout(99, 7);

        ball_buffer[0] = ball->pos.x;
        ball_buffer[1] = ball->pos.y;
        game_sub_status->value = 2000;
        motion->manualMotion(0, 0, 0);

        game_sub_status->resetUptimeTimeout();

        break;

    case 2000:
        if (ball->distance > 370) {
            ball_buffer[0] = ball->pos.x;
            ball_buffer[1] = ball->pos.y;
        }

        if (game_status->value - 128 == status_preparation_kickoff_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 220, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_cornerkick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_1_4, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_goalkick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_freekick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_throwin_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 180, 120, NORMAL_OBSTACLE);
        }

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 150 && ball->distance < 350) {
            counter_bola_keluar_ring++;
        } else {
            counter_bola_keluar_ring = 0;
        }

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 170 && counter_bola_keluar_ring > 4) {
            counter_bola_keluar_ring = 0;
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(2001, 7);
        break;

    case 2001:
        if (ball->distance > 120) {
            ball_buffer[0] = ball->pos.x;
            ball_buffer[1] = ball->pos.y;
        }

        if (game_status->value - 128 == status_preparation_kickoff_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_cornerkick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_1_4, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_goalkick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_freekick_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 120, NORMAL_OBSTACLE);
        } else if (game_status->value - 128 == status_preparation_throwin_away) {
            motion->motionAroundBall(atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 120, NORMAL_OBSTACLE);
        }

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 120 && ball->distance < 90) {
            counter_bola_keluar_ring++;
        } else {
            counter_bola_keluar_ring = 0;
        }

        if (pythagoras(ball->pos.x, ball->pos.y, ball_buffer[0], ball_buffer[1]) > 170 && counter_bola_keluar_ring > 4) {
            counter_bola_keluar_ring = 0;
            game_sub_status->value = 99;
        }
        break;
    case 20:
        /**
         *! AWAY
         * CEK BOLA BERGERAK > 100
         * RULE RADIUS : KICKOFF > 200 | CORNER THROWIN GOALKICK FREEKICK > 300
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */
        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG);
            motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE);
        } else {
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", motion->BallAngletoOurGoal() - 180);
            motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE);
        }

        if (motion->RobotDist2Point(pos_robot_buffer[0], pos_robot_buffer[1]) > 50) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(99, 7);

        break;

    case 3:
        /**
         ** PENALTY
         */
        if (motion->catchingBall(50, 30)) {
            game_sub_status->value = 33;
            game_sub_status->resetUptimeTimeout();
        }
        break;

    case 33:
        stm_data->global_kicker_position = 200;
        motion->hadapGawangVision(0, 0, 15);
        // if (motion->hadapGawangVision(0, 0, 90)) {
        //     game_sub_status->value = 333;
        // }

        game_sub_status->timeout(333, 0.5);
        break;

    case 333: {
        stm_data->global_kicker_position = 200;
        motion->hadapGawangVision(0, 0, 90);

        if (ball->is_caught > 0) {
            //=================================
            //        TENDANGAN PENALTY
            //=================================
            kickerControl(stm_data, KICKER_POSITION_AIM, 50, 200);
        }

        break;
    }
    case 99:
        /**
         * PERSIAPAN MASUK LAYER 2
         */
        *has_prep = 1;
        game_sub_status->value = 100;
        break;
    default:
        break;
    }

    if (game_sub_status->value != 100 && (*has_prep == 0 || *has_prep == 2 || *has_prep == 3)) {
        return;
    }
    //==============================================================================================

    //?================================================
    //?                   LAYER 2
    //?================================================

    *has_prep = 1;

    static MachineState game_sub_sub_status;
    game_sub_sub_status.reentry(0, 0.1);

    static uint8_t prev_sub_sub_state = 0;

    if (ball->distance < 200 && ball->status > 0) {
        obs = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance - 0, NORMAL_OBSTACLE);
    }

    // printf("TEST %d %.2f %.2f\n", obs.status, ball->distance, final_pose->theta);

    // MENENTUKAN STATE
    if (ball->is_caught > 0) {
        // HAS BALL
        game_sub_sub_status.value = 2;
    } else if (ball->status > 0) {
        // FRIEND HAS BALL
        if (obs.status && ball->pos.x > FIELD_X_0 + 100 && ball->pos.x < FIELD_X_1 - 100 && ball->distance > ring_rebut_bola_min && ball->distance < ring_rebut_bola_max && ball->pos.y < FIELD_Y_1 - 125 && ball->pos.y > FIELD_Y_0 + 125 && (final_pose->theta > -15 || final_pose->theta < -165)) {
            // ENEMY HAS BALL
            game_sub_sub_status.value = 1;
        } else if (obs.status && ball->pos.x > FIELD_X_0 + 100 && ball->pos.x < FIELD_X_1 - 100 && ball->distance > ring_rebut_bola_min && ball->distance < ring_rebut_bola_max && ball->pos.y < FIELD_Y_1 - 125 && ball->pos.y > FIELD_Y_0 + 125 && obs.distance - ball->distance) {
            // ENEMY HAS BALL V2
            game_sub_sub_status.value = 1;
        } else {
            // WILD BALL
            game_sub_sub_status.value = 4;
        }
    } else {
        // ROAMING
        game_sub_sub_status.value = 0;
    }

    //=====================================================================================

    // Update posisi bola sebelum hilang, digunakan untuk roaming
    if (ball->status == 2) {
        *bola_terakhir_sebelum_hilang[0] = ball->pos.x;
        *bola_terakhir_sebelum_hilang[1] = ball->pos.y;
    }

    // Reset hysteresis paksa dribble
    // if (game_sub_sub_status.value != 1) {
    //     stm_data->force_dribble_control = 0;
    // }

    // Reset lock musuh
    if (ball->status == 2) {
        *enemy_locked = 0;
    }

    // Lock musuh agar bisa mengikuti posisi lawan, bukan bola
    if ((prev_sub_sub_state == 1 && game_sub_sub_status.value == 0) || (obs.status && game_sub_sub_status.value == 0)) {
        *enemy_locked = 1;
        *enemy_locked_obs = motion->obstacleChecking(motion->RobotAngletoPoint(obs.pos_x, obs.pos_y), 40,
            motion->RobotDist2Point(obs.pos_x, obs.pos_y) + 75, NORMAL_OBSTACLE);
    }

    //=====================================================================================

    printf("GAME MAIN %d\n", game_sub_sub_status.value);

    switch (game_sub_sub_status.value) {
    case 0:
        if (prev_sub_sub_state == 4)
            Roaming(10);
        else
            Roaming(2);
        break;
    case 1:
        EnemyHasBall();
        break;
    case 2:
        HasBall();
        break;
    case 3:
        FriendHasBall();
        break;
    case 4:
        /**
         * ? STATE 0 BLOCK OBSTACLE -> STATE 1 MENGHADAP BOLA
         */
        WildBall();
        break;
    case 69:
        // unused
        motion->manualMotion(0, 0, 0);
        break;
    case 5:
        // unused
        EnemyLock(obs.pos_x, obs.pos_y);
        break;
    }

    prev_sub_sub_state = game_sub_sub_status.value;
    return;
}

void Attacker::GameMainDouble() const
{
    static int prev_n_has_ball;
    static uint64_t pass_timer[2];
    static uint64_t time_update = 0;
    static float ball_buffer[2];
    static obstacle_checking_t obs;
    static float pos_robot_buffer[2];
    static target_goal_t buffer_goal;
    static const int ring_rebut_bola_min = 0;
    static const int ring_rebut_bola_max = 150;

    static target_goal_t buffer_target_goal;

    static float pos_x_sebelum_menerima = 0;
    static float pos_y_sebelum_menerima = 0;

    static float pos_ball_buffer[2];
    static float pos_obs_buffer[2];

    static int16_t counter_lawan_pegang_bola = 0;
    static uint16_t counter_bola_keluar_ring = 0;

    static obstacle_checking_t obs_lawan;

    static float ball_arbritary[2];
    static uint8_t ball_arbritary_update;

    static float jarak_bola_berpindah = 100;

    // OBSTACLE AVOIDANCE MENGHADAP BOLA
    obs = motion->obstacleChecking(motion->RobotAngletoBall(), 30, ball->distance + 50, NORMAL_OBSTACLE);

    game_sub_status->reentry(0, 0.05);
    printf("Attacker::GameMainDouble() %d || %d %d\n", game_sub_status->value, *has_prep, shared_robot_data[bs_data->n_robot_def_1]->robot_condition);

    static uint8_t global_prev_lock_goal_kiri_atau_kanan = 0;
    static uint8_t counter_menggiring_maju = 0;

    // HAS PREP = 0 -> HOME
    // HAS PREP = 1 -> MASUK KE LAYER 2 ATAU DROP BALL
    // HAS PREP = 2 -> AWAY
    // HAS PREP = 3 -> PENALTY

    //*================================================
    //*                   LAYER 1
    //*================================================

    if (*has_prep == 1)
        game_sub_status->value = 100;

    switch (game_sub_status->value) {
    case 0:
        /**
         * MENENTUKAN STATE
         */
        if (*has_prep == 0) // Home
        {
            game_sub_status->value = 1;
        } else if (*has_prep == 2) // Away
        {
            game_sub_status->value = 2;
        } else if (*has_prep == 3) // Special
        {
            game_sub_status->value = 3;
        } else if (*has_prep == 4) // Special
        {
            game_sub_status->value = 4;
        }

        if (ball->status > 0) {
            ball_arbritary[0] = ball->pos.x;
            ball_arbritary[1] = ball->pos.y;
            ball_arbritary_update = 1;
        } else {
            ball_arbritary_update = 0;
        }

        pos_x_sebelum_menerima = final_pose->x;
        pos_y_sebelum_menerima = final_pose->y;

        counter_lawan_pegang_bola = 0;
        counter_bola_keluar_ring = 0;

        *robot_condition = 0;

        stm_data->force_dribble_control = 1;

        game_sub_status->resetUptimeTimeout();
        break;
    case 12:
        if (motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoBall(), 50, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status->value = 1;
        }

        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition != 11) {
            motion->manualMotion(0, 0, 0);
            break;
        }

        pos_x_sebelum_menerima = final_pose->x;
        pos_y_sebelum_menerima = final_pose->y;

        game_sub_status->timeout(1, 3);
        break;
    case 1:
        /**
         *? HOME
         * DIAM -> DEFENDER BELUM MENENDANG
         * RECEIVE BALL -> DEFENDER SUDAH MENENDANG
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */
        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition != 11 && shared_robot_data[bs_data->n_robot_def_1]->robot_condition != 99) {
            motion->receiveBall3(FIELD_X_1_2, FIELD_Y_1_2, 50, 100);
            game_sub_status->timeout(99, 7);
            break;
        }

        if (motion->receiveBall3(FIELD_X_1_2, FIELD_Y_1_2, 190, 100)) {
            if (bs_data->style == 'B') {
                /**
                 * ! =========================================
                 * ! STYLE B
                 * ! TARGET GIRING -> TARGET FINAL POSITIONING
                 * ! =========================================
                 */
                game_sub_status->value = 99;
                break;
            } else {
                if (game_status->value - 128 == status_preparation_kickoff_home) {
                    game_sub_status->value = 99;
                } else {
                    game_sub_status->value = 1010;
                }
            }
        }

        // Safety jika robot terlalu pinggir
        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 80 && final_pose->y < FIELD_Y_0 + 130) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
            printf("KELUAR SAFETY 1\n");
            motion->manualMotion(0, 0, 0);
            game_sub_status->value = 99;
        }

        // Safety jika menerima terlalu jauh
        if (motion->RobotDist2Point(pos_x_sebelum_menerima, pos_y_sebelum_menerima) > 70) {
            printf("KELUAR SAFETY 2\n");
            game_sub_status->value = 99;
        }

        printf("APAKAH MASUK SINI?\n");

        game_sub_status->timeout(99, 4.5);

        break;
    case 1010:
        // printf("LOKASI: %.2f %.2f");
        if (final_pose->y > 600 && final_pose->y < 1000) {
            if (final_pose->x < FIELD_X_1_2) {
                game_sub_status->value = 1011;
            } else {
                game_sub_status->value = 1021;
            }
        } else {
            game_sub_status->value = 99;
        }

        stm_data->force_dribble_control = 1;
        game_sub_status->value = 10301; // langsung saja

        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_2_3, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 50, 60);

        game_sub_status->resetUptimeTimeout();
        break;
    case 1011:
        if (ball->is_caught == 0) {
            motion->catchingBall(190, 100);
            break;
        }
        {
            float error_th = deltaAngle(180, final_pose->theta);
            if (motion->pivotMotion(180, 170)) {
                game_sub_status->value = 1030;
            }
            if (fabs(error_th) < 40) {
                game_sub_status->value = 1030;
            }
        }
        game_sub_status->timeout(99, 9);
        break;
    case 1021:
        if (ball->is_caught == 0) {
            motion->catchingBall(190, 100);
            break;
        }
        {
            float error_th = deltaAngle(0, final_pose->theta);
            if (motion->pivotMotion(0, 170)) {
                game_sub_status->value = 1030;
            }
            if (fabs(error_th) < 40) {
                game_sub_status->value = 1030;
            }
        }
        game_sub_status->timeout(99, 9);
        break;
    case 10301:
        stm_data->force_dribble_control = 1;
        if (ball->is_caught == 0) {
            motion->catchingBall(190, 100);
            break;
        }
        {
            float error_th = deltaAngle(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), final_pose->theta);
            if (motion->pivotMotion(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 170)) {
                game_sub_status->value = 1030;
            }
            if (fabs(error_th) < 25) {
                game_sub_status->value = 1030;
            }
        }
        game_sub_status->timeout(99, 9);
        break;
    case 1030: {
        stm_data->force_dribble_control = 1;
        static int16_t gawang_kiri;
        static int16_t gawang_kanan;

        if (ball->is_caught == 0) {
            motion->catchingBall(190, 100);
            break;
        }

        target_goal_t buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();

        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(335, 1200) : motion->RobotAngletoPoint(455, 1200);
        float target_sudut_min = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(320, 1200) : motion->RobotAngletoPoint(450, 1200);
        float target_sudut_max = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(340, 1200) : motion->RobotAngletoPoint(470, 1200);

        float delta_saya_ke_minimum = deltaAngle(final_pose->theta, target_sudut_min);
        float delta_saya_ke_maksimum = deltaAngle(final_pose->theta, target_sudut_max);
        float delta_minimum_ke_maksimum = deltaAngle(target_sudut_min, target_sudut_max);

        // TOLERANSI
        // ROBOT SUDAH MENGHADAP DI AREA GAWANG MINIMUM MAKSIMUM (TANPA GERAK)
        // if (buffer_target_goal.status_gawang_kosong && delta_saya_ke_minimum < delta_minimum_ke_maksimum && delta_saya_ke_maksimum < delta_minimum_ke_maksimum) {
        //     LogWithColor("red", "SHOT PAKAI TOLERANSI BARU %.2f %.2f || target %.2f || saya %.2f\n",
        //         delta_saya_ke_minimum,
        //         delta_saya_ke_maksimum,
        //         target_sudut_shot,
        //         final_pose->theta);
        //     game_sub_status->value = 1040;
        // }

        float kecepatan_translasi = 100;
        float vx_lapangan = kecepatan_translasi * cosf(target_sudut_shot DEG2RAD);
        float vy_lapangan = kecepatan_translasi * sinf(target_sudut_shot DEG2RAD);

        float vx_local = vx_lapangan * sinf(final_pose->theta DEG2RAD) - vy_lapangan * cosf(final_pose->theta DEG2RAD);
        float vy_local = vx_lapangan * cosf(final_pose->theta DEG2RAD) + vy_lapangan * sinf(final_pose->theta DEG2RAD);

        if (final_pose->y > 950) {
            vx_local = 0;
            vy_local = 0;
        }

        // OUTPUT MOTION
        // ROBOT TIDAK MENGHADAP DI AREA GAWANG MINIMUM MAKSIMUM (NUNGGU GERAK SELESAI)
        if (motion->hadapGawangVision(vx_local, vy_local, 120)) {
            LogWithColor("red", "SHOT PAKAI TOLERANSI BIASA %.2f %.2f || target %.2f || saya %.2f\n",
                delta_saya_ke_minimum,
                delta_saya_ke_maksimum,
                target_sudut_shot,
                final_pose->theta);
            game_sub_status->value = 1040;

            global_prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
            counter_menggiring_maju = 0;
        }
        game_sub_status->timeout(99, 9);
    } break;
    case 1040: {
        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(350, 1200) : motion->RobotAngletoPoint(450, 1200);

        float vx_lapangan = 90 * cosf(target_sudut_shot DEG2RAD);
        float vy_lapangan = 90 * sinf(target_sudut_shot DEG2RAD);

        float vx_local = vx_lapangan * sinf(final_pose->theta DEG2RAD) - vy_lapangan * cosf(final_pose->theta DEG2RAD);
        float vy_local = vx_lapangan * cosf(final_pose->theta DEG2RAD) + vy_lapangan * sinf(final_pose->theta DEG2RAD);

        if (final_pose->y > 950) {
            vx_local = 0;
            vy_local = 0;
        }

        // motion->motionAngleControl(vx_local, vy_local, target_sudut_shot, 20);

        if (*lock_goal_kiri_atau_kanan == global_prev_lock_goal_kiri_atau_kanan) {
            motion->hadapGawangVision(vx_local, vy_local, 160);
        } else {
            motion->motionAngleControl(vx_local, vy_local, target_sudut_shot, 60);
        }

        if (counter_menggiring_maju++ <= 10) {
            break;
        }

        if (ball->is_caught == 0) {
            game_sub_status->value = 99;
        } else {
            //=================================
            //        TENDANGAN GAWANG
            //=================================
            kickerControl(stm_data, NO_AIM, 30);
        }
        game_sub_status->timeout(99, 9);
        break;
    }

        // case 111:
        //     /**
        //      *? HOME
        //      * CATCHING BALL
        //      * NEXT STATE -> 10
        //      */

        //     motion->catchingBall(65, 60);
        //     if (ball->is_caught == 2) {
        //         game_sub_status->value = 1110;
        //     }
        //     if (final_pose->x < FIELD_X_1_2) {
        //         arah_target = 0;
        //     } else {
        //         arah_target = 180;
        //     }

        //     if (ball->is_caught > 0) {
        //         kickerControl(stm_data, OFFBALLWITHDRIBBLE, 2);
        //         game_sub_status->value = 1111;
        //     }
        //     break;

        // case 1110:
        //     /**
        //      *? HOME
        //      * MENGARAHKAN BOLA
        //      * NEXT STATE -> 11
        //      */

        //     if (ball->is_caught > 0) {
        //         kickerControl(stm_data, OFFBALLWITHDRIBBLE, 2);
        //         game_sub_status->value = 1111;
        //     }

        //     if (game_status->value - 128 == status_preparation_cornerkick_home) {
        //         if (motion->pivotMotion(final_pose->theta, 180)) {
        //             game_sub_status->value = 1111;
        //         }
        //     } else if (game_status->value - 128 == status_preparation_goalkick_home) {
        //         if (motion->pivotMotion(final_pose->theta, 180)) {
        //             game_sub_status->value = 1111;
        //         }
        //     } else if (game_status->value - 128 == status_preparation_freekick_home) {
        //         if (motion->pivotMotion(robotAngletoEnemyGoal(*final_pose) - 180, 180)) {
        //             game_sub_status->value = 1111;
        //         }
        //     } else if (game_status->value - 128 == status_preparation_throwin_home) {
        //         if (motion->pivotMotion(final_pose->theta, 180)) {
        //             game_sub_status->value = 1111;
        //         }
        //     } else {
        //         if (motion->pivotMotion(-135, 180)) {
        //             game_sub_status->value = 1111;
        //         }
        //     }

        //     if (ball->is_caught == 0) {
        //         game_sub_status->value = 111;
        //     }
        //     break;

        // case 1111:
        //     /**
        //      *? HOME
        //      * TENDANG BOLA
        //      * NEXT STATE -> 110
        //      */
        //     kickerControl(stm_data, OFFBALLWITHDRIBBLE, 50);
        //     if (ball->is_caught == 0) {
        //         game_sub_status->value = 1112;
        //         game_sub_status->resetUptimeTimeout();
        //     }
        //     break;

        // case 1112:
        //     /**
        //      *? HOME
        //      * CATCHING BALL
        //      * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
        //      */
        //     motion->catchingBall(190, 120, LANGSUNG_AMBIL);

        //     if (ball->is_caught == 2)
        //         game_sub_status->value = 99;

        //     game_sub_status->timeout(99, 6);
        //     break;

    case 2:
        /**
         *! AWAY
         * SELALU MENCATAT POSISI
         * RULE RADIUS : KICKOFF > 200 | CORNER THROWIN GOALKICK FREEKICK > 300
         * NEXT STATE -> 20 (PERSIAPAN MASUK LAYER 2)
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */

        if (ball->distance > 370) {
            ball_arbritary[0] = ball->pos.x;
            ball_arbritary[1] = ball->pos.y;
            ball_arbritary_update = 0;
        } else if (ball->distance > 340) {
            ball_arbritary_update = 0;
        } else if (ball->status > 0) {
            ball_arbritary_update = 1;
        }

        if (bs_data->style == 'M') {
            jarak_bola_berpindah = 50;
        } else {
            jarak_bola_berpindah = 100;
        }

        printf("CASE 2: %.2f\n", jarak_bola_berpindah);

        if (game_status->value - 128 == status_preparation_kickoff_away) {
            if (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) > jarak_bola_berpindah && ball_arbritary_update == 1) {
                game_sub_status->value = 99;
            }
        } else {
            if (pythagoras(ball->pos.x, ball->pos.y, ball_arbritary[0], ball_arbritary[1]) > 100 && ball_arbritary_update == 1) {
                game_sub_status->value = 99;
            }
        }
        pos_robot_buffer[0] = final_pose->x;
        pos_robot_buffer[1] = final_pose->y;
        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            printf("CORNER KICK\n");
            if (motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 300, 180, 70)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
                pos_ball_buffer[0] = ball->pos.x;
                pos_ball_buffer[1] = ball->pos.y;

                obs_lawan = motion->obstacleChecking(motion->RobotAngletoPoint(ball->pos.x, ball->pos.y), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 20, NORMAL_OBSTACLE);
                pos_obs_buffer[0] = obs_lawan.pos_x;
                pos_obs_buffer[1] = obs_lawan.pos_y;
            }
        } else if (game_status->value - 128 == status_preparation_kickoff_away) {
            printf("KICKOFF\n");
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 220, 180, 70)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
                pos_ball_buffer[0] = ball->pos.x;
                pos_ball_buffer[1] = ball->pos.y;

                obs_lawan = motion->obstacleChecking(motion->RobotAngletoPoint(ball->pos.x, ball->pos.y), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 20, NORMAL_OBSTACLE);
                pos_obs_buffer[0] = obs_lawan.pos_x;
                pos_obs_buffer[1] = obs_lawan.pos_y;
            }
        } else {
            printf("NOT CORNER OR KICKOFF\n");
            if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 180, 70)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
                pos_ball_buffer[0] = ball->pos.x;
                pos_ball_buffer[1] = ball->pos.y;

                obs_lawan = motion->obstacleCheckNearestTo(motion->RobotAngletoBall(), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 40, ball->pos.x, ball->pos.y);
                pos_obs_buffer[0] = obs_lawan.pos_x;
                pos_obs_buffer[1] = obs_lawan.pos_y;
            }
        }

        counter_lawan_pegang_bola = 0;
        counter_bola_keluar_ring = 0;

        //! KELUAR KARENA TIMEOUT 7 DETIK
        game_sub_status->timeout(200, 7);

        break;

    case 20:
        /**
         *! AWAY
         * CEK BOLA BERGERAK > 100
         * RULE RADIUS : KICKOFF > 200 | CORNER THROWIN GOALKICK FREEKICK > 300
         * NEXT STATE -> 99 (PERSIAPAN MASUK LAYER 2)
         */
        // printf("buff: %.2f %.2f || now: %.2f %.2f -> %.2f\n",
        //     pos_robot_buffer[0], pos_robot_buffer[1],
        //     final_pose->x, final_pose->y,
        //     pythagoras(pos_robot_buffer[0], pos_robot_buffer[1], final_pose->x, final_pose->y));
        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", motion->BallAngletoEnemyGoal() - 180);
            motion->motionAroundBall(motion->BallAngletoEnemyGoal() - 180, motion->RobotAngletoBall(), 300, 120, 70);
        } else if (game_status->value - 128 == status_preparation_kickoff_away) {
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", motion->BallAngletoOurGoal() - 180);
            motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 220, 120, 70);
        } else {
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", motion->BallAngletoOurGoal() - 180);
            motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 300, 120, 70);
        }

        // {
        //     static uint16_t counter_masuk = 0;
        //     obs_lawan = motion->obstacleCheckNearestTo(motion->RobotAngletoBall(), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 40, ball->pos.x, ball->pos.y);

        //     float jarak = pythagoras(obs_lawan.pos_x, obs_lawan.pos_y, ball->pos.x, ball->pos.y);

        //     uint8_t bola_dipegang_lawan = 0;

        //     if (jarak < 25) {
        //         counter_masuk = fminf(50, counter_masuk + 1);
        //     } else {
        //         counter_masuk = fmaxf(0, counter_masuk - 2);
        //     }

        //     if (counter_masuk > 49) {
        //         bola_dipegang_lawan = 1;
        //     }
        // }

        obs_lawan = motion->obstacleCheckNearestTo(motion->RobotAngletoBall(), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 40, ball->pos.x, ball->pos.y);
        // obs_lawan = motion->obstacleChecking(motion->RobotAngletoBall(), 25, motion->RobotDist2Point(ball->pos.x, ball->pos.y) + 40, NORMAL_OBSTACLE);

        printf("tunggu %d %d: %.2f %.2f || %.2f %.2f || %.2f || %.2f %d\n", counter_lawan_pegang_bola, counter_bola_keluar_ring, pos_ball_buffer[0], pos_ball_buffer[1], ball->pos.x, ball->pos.y, pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]), pythagoras(obs_lawan.pos_x, obs_lawan.pos_y, ball->pos.x, ball->pos.y), obs_lawan.status);

        if (pythagoras(obs_lawan.pos_x, obs_lawan.pos_y, ball->pos.x, ball->pos.y) < 25) {
            counter_lawan_pegang_bola = fminf(50, counter_lawan_pegang_bola + 1);
        } else {
            counter_lawan_pegang_bola = fmaxf(0, counter_lawan_pegang_bola - 2);
        }

        if (bs_data->style == 'M') {
            jarak_bola_berpindah = 50;
        } else {
            jarak_bola_berpindah = 100;
        }

        printf("CASE 20: %.2f\n", jarak_bola_berpindah);

        if (game_status->value - 128 == status_preparation_kickoff_away) {
            if (pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]) > jarak_bola_berpindah) {
                counter_bola_keluar_ring++;
            } else {
                counter_bola_keluar_ring = 0;
            }
        } else {
            if (pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]) > 50) {
                counter_bola_keluar_ring++;
            } else {
                counter_bola_keluar_ring = 0;
            }
        }

        if (game_status->value - 128 == status_preparation_kickoff_away) {
            if (counter_bola_keluar_ring > 4) {
                counter_lawan_pegang_bola = 0;
                counter_bola_keluar_ring = 0;
                game_sub_status->value = 99;
            }
        } else {
            if ((counter_lawan_pegang_bola > 49 && pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]) > 20) || counter_bola_keluar_ring > 4) {
                counter_lawan_pegang_bola = 0;
                counter_bola_keluar_ring = 0;
                game_sub_status->value = 99;
            }
        }

        game_sub_status->timeout(200, 7);

        // if (pythagoras(pos_robot_buffer[0], pos_robot_buffer[1], final_pose->x, final_pose->y) > 100) {
        //     game_sub_status->value = 99;
        // }
        break;

    case 200:
        printf("CASE200: %d | %.2f %.2f | %d %d\n", shared_robot_data[bs_data->n_robot_def_1]->robot_condition, pythagoras(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y, ball->pos.x, ball->pos.y), pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]), counter_lawan_pegang_bola, counter_bola_keluar_ring);
        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition == 99) {
            game_sub_status->value = 99;
            break;
        }

        // Ketika teman memegang bola
        if (bs_data->n_has_ball > 0 && ball->is_caught == 0) {
            game_sub_status->value = 201;
            break;
        } else if (pythagoras(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y, ball->pos.x,
                       ball->pos.y)
            < 150) {
            game_sub_status->value = 201;
            break;
        }

        // Jika bola tiba-tiba bergerak
        if (pythagoras(ball->pos.x, ball->pos.y, pos_ball_buffer[0], pos_ball_buffer[1]) > 150) {
            counter_bola_keluar_ring++;
        } else {
            counter_bola_keluar_ring = 0;
        }

        if (motion->RobotDist2Point(ball->pos.x, ball->pos.y) < pythagoras(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y, ball->pos.x, ball->pos.y)) {
            if (game_status->value - 128 == status_preparation_kickoff_away) {
                if (counter_bola_keluar_ring > 25) {
                    counter_lawan_pegang_bola = 0;
                    counter_bola_keluar_ring = 0;
                    game_sub_status->value = 99;
                }
            } else {
                if (counter_lawan_pegang_bola > 49 || counter_bola_keluar_ring > 4) {
                    counter_lawan_pegang_bola = 0;
                    counter_bola_keluar_ring = 0;
                    game_sub_status->value = 99;
                }
            }
        }

        // Bersiap siap untuk pindah ke tempat untuk diumpan
        if (shared_robot_data[bs_data->n_robot_def_1]->pose.x > FIELD_X_1_2) {
            motion->motionToPoint(FIELD_X_1_4, FIELD_Y_1_2, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 180, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        } else {
            motion->motionToPoint(FIELD_X_3_4, FIELD_Y_1_2, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 180, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        }

        break;

    case 201:
        // Ketika teman mengumpan bola
        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition == 11) {
            game_sub_status->value = 202;
            game_sub_status->resetUptimeTimeout();
            break;
        }

        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition == 99) {
            game_sub_status->value = 99;
            break;
        }

        // Mencari posisi untuk diumpan oleh defender
        if (motion->RobotDist2Point(*titik_final_positioning[0], *titik_final_positioning[1]) < 100) {
            motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 50, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        } else {
            motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y), 120, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        }

        break;

    case 202:
        // Terima bola
        if (motion->receiveBall3(FIELD_X_1_2, FIELD_Y_1_2, 190, 100)) {
            stm_data->force_dribble_control = 1;
            game_sub_status->value = 99;
        }

        if (shared_robot_data[bs_data->n_robot_def_1]->robot_condition == 99) {
            game_sub_status->value = 99;
            break;
        }

        // Ketika terima bola gagal, berarti bola bisa di-intercept lawan, segera ke game main
        game_sub_status->timeout(99, 1.5);

        break;

    case 3:
        /**
         ** PENALTY
         */
        if (motion->catchingBall(50, 30)) {
            game_sub_status->value = 33;
            game_sub_status->resetUptimeTimeout();
        }
        break;

    case 33:
        stm_data->global_kicker_position = 200;
        motion->hadapGawangVision(0, 0, 15);
        // if (motion->hadapGawangVision(0, 0, 90)) {
        //     game_sub_status->value = 333;
        // }

        game_sub_status->timeout(333, 0.5);
        break;

    case 333: {
        stm_data->global_kicker_position = 200;
        motion->hadapGawangVision(0, 0, 90);

        if (ball->is_caught > 0) {
            //=================================
            //        TENDANGAN PENALTY
            //=================================
            kickerControl(stm_data, KICKER_POSITION_AIM, 50, 200);
        }

        break;
    }

    case 4:
        motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, 160, motion->RobotAngletoBall(), 250, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);

        // Ketika bola sudah ditendang
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) > 250 && ball->pos.y > FIELD_Y_0 + 50 && ball->pos.y < FIELD_Y_1_4) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(200, 7);
        break;

    case 99:
        /**
         * PERSIAPAN MASUK LAYER 2
         */
        *has_prep = 1;
        game_sub_status->value = 100;
        break;
    }

    if (game_sub_status->value != 100 && (*has_prep == 0 || *has_prep == 2 || *has_prep == 3 || *has_prep == 4)) {
        return;
    }

    //=================================================================

    //?================================================
    //?                   LAYER 2
    //?================================================

    *has_prep = 1;

    *robot_condition = 99;

    static MachineState game_sub_sub_status;
    game_sub_sub_status.reentry(0, 0.1);

    static uint8_t prev_sub_sub_state = 0;

    // if (ball->distance < 200 && ball->status > 0) {
    //     obs = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance - 0, NORMAL_OBSTACLE);
    // }

    // COBAAA
    if (ball->distance < 300 && ball->status > 0) {
        obs = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance - 0, NORMAL_OBSTACLE);
    }

    // MENENTUKAN STATE
    if (ball->is_caught > 0) {
        // HAS BALL
        game_sub_sub_status.value = 2;
    } else if (bs_data->n_has_ball > 0 && ball->is_caught == 0) {
        // FRIEND HAS BALL
        game_sub_sub_status.value = 3;
    } else if (ball->status > 0) {
        if (obs.status && ball->pos.x > FIELD_X_0 + 100 && ball->pos.x < FIELD_X_1 - 100 && ball->distance > ring_rebut_bola_min && ball->distance < ring_rebut_bola_max && (final_pose->theta > -15 || final_pose->theta < -165) && pythagoras(obs.pos_x, obs.pos_y, shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y) > 100) {
            // ENEMY HAS BALL
            game_sub_sub_status.value = 1;
        } else if (obs.status && ball->pos.x > FIELD_X_0 + 100 && ball->pos.x < FIELD_X_1 - 100 && ball->distance > ring_rebut_bola_min && ball->distance < ring_rebut_bola_max + 250 && ball->pos.y < FIELD_Y_1 - 125 && ball->pos.y > FIELD_Y_0 + 125 && obs.distance - ball->distance) {
            // ENEMY HAS BALL V2
            game_sub_sub_status.value = 1;
        } else {
            // WILD BALL
            game_sub_sub_status.value = 4;
        }
    } else {
        // ROAMING
        game_sub_sub_status.value = 0;
    }

    //==========================================================================

    // Update posisi bola sebelum hilang, digunakan untuk roaming
    if (ball->status == 2) {
        *bola_terakhir_sebelum_hilang[0] = ball->pos.x;
        *bola_terakhir_sebelum_hilang[1] = ball->pos.y;
    }

    // Reset hysteresis paksa dribble
    // if (game_sub_sub_status.value != 1) {
    //     stm_data->force_dribble_control = 0;
    // }

    // Reset lock musuh
    if (ball->status == 2 || final_pose->y < FIELD_Y_1_4) {
        *enemy_locked = 0;
    }

    // Lock musuh agar bisa mengikuti posisi lawan, bukan bola
    if ((prev_sub_sub_state == 1 && game_sub_sub_status.value == 0) || (obs.status && game_sub_sub_status.value == 0 && pythagoras(obs.pos_x, obs.pos_y, shared_robot_data[bs_data->n_robot_def_1]->pose.x, shared_robot_data[bs_data->n_robot_def_1]->pose.y) > 100)) {
        *enemy_locked = 1;
        *enemy_locked_obs = motion->obstacleChecking(motion->RobotAngletoPoint(obs.pos_x, obs.pos_y), 40,
            motion->RobotDist2Point(obs.pos_x, obs.pos_y) + 75, NORMAL_OBSTACLE);
    }

    else if (prev_sub_sub_state == 3 && game_sub_sub_status.value == 4 && shared_robot_data[bs_data->n_robot_def_1]->robot_condition == is_passing_inside_state) {
        *receive_lock = 1;
    }

    //=====================================================================================

    // printf("MAIN: %d\n", game_sub_sub_status.value);

    switch (game_sub_sub_status.value) {
    case 0:
        if (prev_sub_sub_state == 4)
            Roaming(10);
        else
            Roaming(2);
        break;
    case 1:
        EnemyHasBall();
        break;
    case 2:
        HasBall();
        break;
    case 3:
        FriendHasBall();
        break;
    case 4:
        /**
         * ? STATE 0 BLOCK OBSTACLE -> STATE 1 MENGHADAP BOLA
         */
        WildBall();
        break;
    case 69:
        // unused
        motion->manualMotion(0, 0, 0);
        break;
    case 5:
        // unused
        EnemyLock(obs.pos_x, obs.pos_y);
        break;
    }

    prev_sub_sub_state = game_sub_sub_status.value;
    return;
}
void Attacker::GameMainTriple() const { }
void Attacker::GameMainQuadruple() const { }

void Attacker::HasBall() const
{
    static float target_x;
    static float target_y;
    static float buffer_x;
    static float buffer_y;

    static obstacle_checking_t obstacle;

    static target_goal_t buffer_target_goal;

    static float pos_buffer[2];

    //-------------
    // static const float titik_target_x[4] = {130, 250, 550, 670};
    // static const float titik_target_y[4] = {950, 800, 800, 950};
    static const float titik_target_x[4] = { 150, 330, 470, 650 };
    static const float titik_target_y[4] = { 950, 850, 850, 950 };
    static int idx_target;
    static int init_idx;

    static uint16_t counter_menggiring_sebelum_tendang = 0;
    static float pos_x_sebelum_gerak_maju = 0;
    static float pos_y_sebelum_gerak_maju = 0;
    static float kecepatan_translasi_maju = 100;

    static uint8_t prev_lock_goal_kiri_atau_kanan = 0; // kiri 1, kanan 2

    //========== TUNINGABLE VARIABLE ===========
    static const float vel_with_ball_obs_linear = 120;
    static const float vel_with_ball_obs_angular = 120;
    static const float vel_with_ball_without_obs_linear = 120;
    static const float vel_with_ball_without_obs_angular = 120;
    static const float vel_with_ball_penalty_zone_linear = 101;
    static const float vel_with_ball_penalty_zone_angular = 100;
    //=========================================

    static float kecepatan_mundur = -150;

    static MachineState has_ball_state;
    has_ball_state.reentry(0, 0.04);

    LogWithColor("green", "HAS BALL %d POS [%.2f %.2f]\n", has_ball_state.value,
        final_pose->x, final_pose->y);

    if (final_pose->x > FIELD_X_0 + 20 && final_pose->x < FIELD_X_1_2)
        *hisap_dribble_lama = 0;
    else if (final_pose->x < FIELD_X_1 - 20 && final_pose->x > FIELD_X_1_2)
        *hisap_dribble_lama = 0;
    else if (final_pose->x < FIELD_X_0 + 20)
        *hisap_dribble_lama = 1;
    else if (final_pose->x > FIELD_X_1 - 20)
        *hisap_dribble_lama = 1;

    static uint8_t prev_n_has_ball = bs_data->n_has_ball;
    prev_n_has_ball = bs_data->n_has_ball;

    switch (has_ball_state.value) {
    case 0:
        /**
         * pos_buffer[2] -> mencatat lokasi terakhir memegang bola local
         * pos_buffer_robot_has_ball[2] -> mencatat lokasi terakhir memegang bola global
         * buffer_ -> buffer target awal (case 0)
         * target_ -> mencatat target sekarang
         */
        if (final_pose->x < FIELD_X_1_2) {
            if (final_pose->y > 800) {
                /**
                 * [130, 950]
                 */
                idx_target = 0;
                init_idx = 0;
            } else {
                /**
                 * [250, 750]
                 */
                idx_target = 1;
                init_idx = 1;
            }
        } else {
            if (final_pose->y > 800) {
                /**
                 * [670, 950]
                 */
                idx_target = 3;
                init_idx = 3;
            } else {
                /**
                 * [550, 750]
                 */
                idx_target = 2;
                init_idx = 2;
            }
        }

        buffer_x = titik_target_x[idx_target];
        buffer_y = titik_target_y[idx_target];
        target_x = titik_target_x[idx_target];
        target_y = titik_target_y[idx_target];

        pos_buffer[0] = final_pose->x;
        pos_buffer[1] = final_pose->y;

        *pos_buffer_robot_has_ball[0] = final_pose->x;
        *pos_buffer_robot_has_ball[1] = final_pose->y;

        has_ball_state.value = 1;

        counter_menggiring_sebelum_tendang = 0;

        kecepatan_mundur = -150;

        break;

    case 1:
        /**
         * CEK KONDISI ROBOT
         * JIKA TERLALU KANAN KE KIRI DULU 60 CM
         * JIKA TERLALU KIRI KE KANAN DULU 60 CM
         * JIKA DI HADANG OBSTACLE MUNDUR DULU 40 CM
         */
        if (final_pose->x < FIELD_X_0 + 100)
            has_ball_state.value = 2;
        else if (final_pose->x > FIELD_X_1 - 100)
            has_ball_state.value = 3;
        else {
            obstacle = motion->obstacleChecking(final_pose->theta, 25, 120, NORMAL_OBSTACLE);
            if (obstacle.status)
                has_ball_state.value = 4;
            else {
                has_ball_state.value = 10;
                has_ball_state.resetUptimeTimeout();
            }
        }
        break;

    case 2:
        // GERAK KE KANAN 60 CM
        if (motion->motionToPoint(pos_buffer[0] + 60, final_pose->y, final_pose->theta, 120, 90))
            has_ball_state.value = 10;

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 60)
            has_ball_state.value = 10;

        has_ball_state.resetUptimeTimeout();
        break;

    case 3:
        // GERAK KE KIRI 60 CM
        if (motion->motionToPoint(pos_buffer[0] - 60, final_pose->y, final_pose->theta, 120, 90))
            has_ball_state.value = 10;

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 60)
            has_ball_state.value = 10;

        has_ball_state.resetUptimeTimeout();
        break;

    case 4:

    {

        // MUNDUR SEJAUH 40 CM KARENA OBS DI JARAK 120 CM
        motion->manualMotion(0, fminf(-50, kecepatan_mundur += 0.5), 0);
        // motion->manualMotion(0, -150, 0);
        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 70) {
            if (final_pose->y > 900)
                has_ball_state.value = 10;
            else
                has_ball_state.value = 44;
        }
        has_ball_state.resetUptimeTimeout();

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 68) {
            stm_data->force_dribble_control = 1;
            printf("MASUK SINI\n");
        } else
            stm_data->force_dribble_control = 2;

        break;
    }

    case 44: {
        // stm_data->force_dribble_control = 2;

        static uint16_t counter_update_obs_lawan = 0;
        static obstacle_checking_t obs_lawan = motion->obstacleChecking(final_pose->theta, 35, 200, NORMAL_OBSTACLE);
        float target_sudut_pivot = -90;

        if (counter_update_obs_lawan >= 10) {
            obs_lawan = motion->obstacleChecking(final_pose->theta, 35, 200, NORMAL_OBSTACLE);
        }

        if (obs_lawan.status) {
            target_sudut_pivot = obs_lawan.angle - 180;
        }

        if (motion->pivotMotion(target_sudut_pivot, 150)) {
            has_ball_state.value = 10;
        }

        float error_th = deltaAngle(target_sudut_pivot, final_pose->theta);
        if (fabs(error_th) < 40) {
            has_ball_state.value = 10;
        }

        counter_update_obs_lawan++;
        break;
    }

    case 10: {
        /**
         * JALAN KE BUFFER SAMBIL CEK KONDISI OBSTACLE
         * STYLE B LANGSUNG PINDAH CASE LANGSUNG HADAP KE GAWANG
         * STYLE E MENGGUNAKAN TITIK FINAL SEBAGAI TARGET BARU
         */
        stm_data->force_dribble_control = 1;

        if (bs_data->style == 'B') {
            /**
             * ! =========================================
             * ! STYLE B
             * ! TARGET GIRING -> TARGET FINAL POSITIONING
             * ! =========================================
             */
            has_ball_state.value = 20;
            break;
        }

        float kecepatan_memindah_target = 0;

        float jarak_cek_obstacle = 250;
        // if (motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1) < 250)
        jarak_cek_obstacle = motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1) - 80;

        static uint8_t counter_tidak_ada_obs = 0;
        static uint8_t counter_ada_obs = 0;
        static float sudut_kiri_gawang = motion->RobotAngletoPoint(250, 1200);
        static float sudut_kanan_gawang = motion->RobotAngletoPoint(550, 1200);
        static float sudut_dinamis = deltaAngle(sudut_kiri_gawang, sudut_kanan_gawang);

        while (sudut_dinamis > 360)
            sudut_dinamis -= 360;
        while (sudut_dinamis < 0)
            sudut_dinamis += 360;

        obstacle_checking_t obs_depan = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 400, NORMAL_OBSTACLE);
        obstacle_checking_t obs_belakang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_0), 25, 120, NORMAL_OBSTACLE);

        // JALAN MENUJU BUFFER DAN JUGA CEK OBSTACLE KE ARAH GAWANG
        // sudut_dinamis = 35; // HARDCODE
        obstacle = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), sudut_dinamis, 120, NORMAL_OBSTACLE);

        // MENENTUKAN KETINGGIAN PENENDANG
        stm_data->global_kicker_position = motion->AutomaticKickerPos(40);

        if (obstacle.status == 0) {
            counter_tidak_ada_obs++;
            counter_ada_obs = 0;
        } else {
            counter_tidak_ada_obs = 0;
            counter_ada_obs++;
        }

        LogWithColor("yellow", "%d %d buff target: %.2f %.2f || target: %.2f %.2f || pose: %.2f %.2f || idx %d || obs status: %d jarak ke target: %.2f || [sudut dinamis: %.2f jarak cek obs: %.2f] %.2f %.2f %.2f %.2f\n",
            obs_depan.status, obs_belakang.status,
            buffer_x, buffer_y, target_x, target_y,
            final_pose->x, final_pose->y,
            idx_target,
            obstacle.status, motion->RobotDist2Point(titik_target_x[idx_target], titik_target_y[idx_target]),
            sudut_dinamis, jarak_cek_obstacle, obstacle.pos_x, obstacle.pos_y, obstacle.distance, deltaAngle(final_pose->theta, obstacle.angle));

        // MOTION KE BUFFER DENGAN MEMBALIKI LAWAN KETIKA JAUH
        if (final_pose->y < 900) {
            if (obs_depan.status) {
                if (final_pose->x > FIELD_X_1_2)
                    motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), vel_with_ball_obs_linear, vel_with_ball_obs_angular, NORMAL_OBSTACLE | INVERT_OBS);
                else
                    motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), vel_with_ball_obs_linear, vel_with_ball_obs_angular, NORMAL_OBSTACLE | INVERT_OBS);
            } else {
                if (final_pose->x > FIELD_X_1_2)
                    motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), vel_with_ball_without_obs_linear, vel_with_ball_without_obs_angular, NORMAL_OBSTACLE | INVERT_OBS);
                else
                    motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), vel_with_ball_without_obs_linear, vel_with_ball_without_obs_angular, NORMAL_OBSTACLE | INVERT_OBS);
            }
        } else {
            motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), vel_with_ball_penalty_zone_linear, vel_with_ball_penalty_zone_angular, NORMAL_OBSTACLE);
        }

        // JIKA ADA OBSTACLE DAN JARAK TARGET < 70 DAN BUKAN STYLE E
        if (obstacle.status == 1 && motion->RobotDist2Point(titik_target_x[idx_target], titik_target_y[idx_target]) < 70 && bs_data->style != 'E') {
            idx_target++;
            // DEPAN KIRI, BELAKANG KIRI | BELAKANG KANAN, DEPAN KANAN
            //{ 130,        250          | 550,            670 };
            //{ 950,        750          | 750,            950 };
            // DEPAN KIRI <-> BELAKANG KIRI
            // DEPAN KANAN <-> BELAKANG KANAN
            // if (init_idx <= 1) {
            //     if (idx_target > 1)
            //         idx_target = 0;
            // } else if (init_idx >= 2) {
            //     if (idx_target > 3)
            //         idx_target = 2;
            // }

            if (idx_target >= 4)
                idx_target = 0;

            target_x = titik_target_x[idx_target];
            target_y = titik_target_y[idx_target];

            LogWithColor("purple", " GANTI TITIK KE %.2f %.2f\n", target_x, target_y);
            buzzerControl(stm_data, 20, 10);
        } else if (bs_data->style == 'E') {
            /**
             * ! =========================================
             * ! STYLE E
             * ! TARGET GIRING -> TARGET FINAL POSITIONING
             * ! =========================================
             */
            target_x = *titik_final_positioning[0];
            target_y = *titik_final_positioning[1];
        }

        //! KELUAR KARENA AREA TENDANG
        if (final_pose->y > 670 && final_pose->y < 970 && final_pose->x > 100 && final_pose->x < 700 && obstacle.status == 0 && motion->RobotDist2Point(titik_target_x[idx_target], titik_target_y[idx_target]) < 50) {
            pos_x_sebelum_gerak_maju = final_pose->x;
            pos_y_sebelum_gerak_maju = final_pose->y;
            has_ball_state.value = 20;
            LogWithColor("red", "KELUAR KARENA DIRASA GK ADA OBSTACLE (belakang)\n");
            buzzerControl(stm_data, 20, 10);
        } else if (final_pose->y > FIELD_Y_3_4 && final_pose->y < 970 && final_pose->x > 100 && final_pose->x < 700 && obstacle.status == 0 && motion->RobotDist2Point(titik_target_x[idx_target], titik_target_y[idx_target]) < 70) {
            pos_x_sebelum_gerak_maju = final_pose->x;
            pos_y_sebelum_gerak_maju = final_pose->y;
            has_ball_state.value = 20;
            LogWithColor("red", "KELUAR KARENA DIRASA GK ADA OBSTACLE (depan)\n");
            buzzerControl(stm_data, 20, 10);
        }

        //! KELUAR KARENA GIRING LEBIH DARI 3M
        static uint16_t threshold_jarak = 300;
        /**
         * ! =========================================
         * ! STYLE G
         * ! THRESHOLD SAFETY
         * ! =========================================
         */
        if (bs_data->style == 'G' || bs_data->style == 'C' || bs_data->style == 'M') {
            threshold_jarak = 220;
        } else {
            threshold_jarak = 320;
        }

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > threshold_jarak) {
            if (obs_depan.status) {
                pos_x_sebelum_gerak_maju = final_pose->x;
                pos_y_sebelum_gerak_maju = final_pose->y;
                has_ball_state.value = 20;
                LogWithColor("red", "KELUAR KARENA 300 CM\n");
                buzzerControl(stm_data, 20, 10);
            } else if (final_pose->y > 600 && final_pose->y < 970 && // 1000 terlalu mepet.. meskipun sakjane gk masuk
                final_pose->x > 100 && final_pose->x < 700) {
                pos_x_sebelum_gerak_maju = final_pose->x;
                pos_y_sebelum_gerak_maju = final_pose->y;
                has_ball_state.value = 20;
                LogWithColor("red", "KELUAR KARENA 300 CM dan DALAM AREA TENDANG\n");
                buzzerControl(stm_data, 20, 10);
            } else {
                has_ball_state.value = 15;
                LogWithColor("red", "LEPAS BOLA SEBENTAR\n");
                buzzerControl(stm_data, 20, 10);
            }
        }

        kecepatan_memindah_target = 2;

        float dx = target_x - buffer_x;
        float dy = target_y - buffer_y;
        float r = fminf(kecepatan_memindah_target, sqrtf(dx * dx + dy * dy));
        float a = atan2f(dy, dx);

        buffer_x += r * cosf(a);
        buffer_y += r * sinf(a);

        counter_menggiring_sebelum_tendang = 0;

        //! KELUAR KARENA TIMEOUT
        has_ball_state.timeout(20, 10);
        break;
    }
    case 12: {
        stm_data->force_dribble_control = 1;

        obstacle_checking_t obs_depan = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 400, NORMAL_OBSTACLE);
        obstacle_checking_t obs_gawang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 135, 100, NORMAL_OBSTACLE);
        obstacle_checking_t obs_full = motion->obstacleChecking(final_pose->theta, 180, 200, NORMAL_OBSTACLE);

        if (obs_full.status) {
            if (motion->motionAroundPointAngle(obs_full.pos_x, obs_full.pos_y, atan2f(obs_full.pos_y - FIELD_Y_1, obs_full.pos_x - FIELD_X_1_2) RAD2DEG,
                    motion->RobotAngletoPoint(obs_full.pos_x, obs_full.pos_y) - 180, 75, 120, 120, NORMAL_OBSTACLE)) {
                has_ball_state.value = 20;
            }
        }

        if (obs_gawang.status == 0 || obs_full.status == 0) {
            has_ball_state.value = 20;
        }

        if (bs_data->style == 'G' /* || bs_data->style == 'C'*/) {
            if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 220) {
                if (obs_depan.status) {
                    pos_x_sebelum_gerak_maju = final_pose->x;
                    pos_y_sebelum_gerak_maju = final_pose->y;
                    has_ball_state.value = 20;
                    LogWithColor("red", "KELUAR KARENA 300 CM\n");
                    buzzerControl(stm_data, 20, 10);
                } else if (final_pose->y > 600 && final_pose->y < 970 && // 1000 terlalu mepet.. meskipun sakjane gk masuk
                    final_pose->x > 100 && final_pose->x < 700) {
                    pos_x_sebelum_gerak_maju = final_pose->x;
                    pos_y_sebelum_gerak_maju = final_pose->y;
                    has_ball_state.value = 20;
                    LogWithColor("red", "KELUAR KARENA 300 CM dan DALAM AREA TENDANG\n");
                    buzzerControl(stm_data, 20, 10);
                } else {
                    has_ball_state.value = 15;
                    LogWithColor("red", "LEPAS BOLA SEBENTAR\n");
                    buzzerControl(stm_data, 20, 10);
                }
            }
        }

        break;
    }
    case 15: {
        // obstacle_checking_t obs_terdekat = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 200, NORMAL_OBSTACLE);

        // if (ball->is_caught > 0) {
        //     if (deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1)) < 30) {
        //         motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 120, 30);
        //     } else {
        //         motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 60, 80);
        //     }
        // } else {
        //     motion->catchingBall(150, 120, LANGSUNG_AMBIL); // SAFETY LANGSUNG
        // }

        // if (deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1)) < 15) {
        //     kickerControl(stm_data, OFFBALLWITHDRIBBLE, 1);
        // }

        motion->manualMotion(0, 0, 0);
        kickerControl(stm_data, OFFBALLWITHDRIBBLE, 1);

        has_ball_state.timeout(20, 10);
        break;
    }

    case 20: {
        /**
         * MENGHADAPKAN KE GAWANG
         * JIKA ERROR KECIL -> RobotAngletoPoint
         * JIKA ERROR BESAR -> pivotMotion -> RobotAngletoPoint
         */
        stm_data->force_dribble_control = 1;
        stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        obstacle_checking_t obs_gawang;
        if (final_pose->y > 900) {
            obs_gawang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 100, NORMAL_OBSTACLE);
        } else {
            obs_gawang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 90, 100, NORMAL_OBSTACLE);
        }
        if (obs_gawang.status && bs_data->style != 'B') {
            printf("%.2f %.2f %.2f %.2f %.2f\n", obs_gawang.pos_x, obs_gawang.pos_y, obs_gawang.distance, obs_gawang.angle, deltaAngle(final_pose->theta, obs_gawang.angle));

            if (obs_gawang.pos_y < FIELD_Y_1 + 70 && final_pose->y < 900) {
                has_ball_state.value = 12;
                break;
            }
        }

        // Safety ketika terlalu depan
        if (final_pose->y > 940) {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_3_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 80, 90)) {
                has_ball_state.value = 30;
            }
            break;
        }

        float min_tolerance = motion->RobotAngletoPoint(300, 1200);
        float max_tolerance = motion->RobotAngletoPoint(500, 1200);

        float delta_sudut = deltaAngle(min_tolerance, max_tolerance) * 0.5;

        printf("AHH: %.2f %.2f %.2f || %.2f %.2f %.2f\n", min_tolerance, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), max_tolerance, delta_sudut, final_pose->theta, deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1)));

        // ERROR KE TENGAH GAWANG < 30 DEG (DIANGGAP HAMPIR TEPAT) (SEBENTAR)
        if (deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1)) < delta_sudut) {
            buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();
            LogWithColor("red", "TRANSISI GOAL PAKE TOLERANSI ERROR KECIL || target %d -> %.2f %.2f || min %.2f %.2f || max %.2f %.2f || %.2f %.2f %.2f %.2f %.2f\n",
                buffer_target_goal.status_gawang_kosong,
                buffer_target_goal.goal_tengah[0], buffer_target_goal.goal_tengah[1],
                buffer_target_goal.goal_kiri[0], buffer_target_goal.goal_kiri[1],
                buffer_target_goal.goal_kanan[0], buffer_target_goal.goal_kanan[1], min_tolerance, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), max_tolerance, delta_sudut, final_pose->theta);

            float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(460, 1200);

            has_ball_state.value = 30;
            counter_menggiring_sebelum_tendang = 0;

            // break;
        }

        // float target_sudut_min = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(450, 1200);

        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(350, 1200) : motion->RobotAngletoPoint(455, 1200);

        float kecepatan_translasi = 100;
        float vx_lapangan = kecepatan_translasi * cosf(target_sudut_shot DEG2RAD);
        float vy_lapangan = kecepatan_translasi * sinf(target_sudut_shot DEG2RAD);

        float vx_local = vx_lapangan * sinf(final_pose->theta DEG2RAD) - vy_lapangan * cosf(final_pose->theta DEG2RAD);
        float vy_local = vx_lapangan * cosf(final_pose->theta DEG2RAD) + vy_lapangan * sinf(final_pose->theta DEG2RAD);

        if (bs_data->style == 'B') {
            /**
             * ! =========================================
             * ! STYLE B
             * ! TARGET GIRING -> TARGET FINAL POSITIONING
             * ! =========================================
             */
            vx_local = 0;
            vy_local = 0;
        }

        // MENGHADAP KE TENGAH GAWANG KARENA ERROR MASIH BESAR (NUNGGU GERAK SELESAI)
        // if (motion->pivotMotion(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 160))
        // {
        //     buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();
        //     LogWithColor("red", "TRANSISI GOAL PAKE TOLERANSI ERROR BESAR || target %d -> %.2f %.2f || min %.2f %.2f || max %.2f %.2f\n",
        //                  buffer_target_goal.status_gawang_kosong,
        //                  buffer_target_goal.goal_tengah[0], buffer_target_goal.goal_tengah[1],
        //                  buffer_target_goal.goal_kiri[0], buffer_target_goal.goal_kiri[1],
        //                  buffer_target_goal.goal_kanan[0], buffer_target_goal.goal_kanan[1]);

        //     float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(460, 1200);

        //     has_ball_state.value = 30;

        //     counter_menggiring_sebelum_tendang = 0;

        //     break;
        // }

        if (motion->motionAngleControlPivot(vx_local * 0.3, vy_local, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 160)) {
            buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();
            LogWithColor("red", "TRANSISI GOAL PAKE TOLERANSI ERROR BESAR || target %d -> %.2f %.2f || min %.2f %.2f || max %.2f %.2f\n",
                buffer_target_goal.status_gawang_kosong,
                buffer_target_goal.goal_tengah[0], buffer_target_goal.goal_tengah[1],
                buffer_target_goal.goal_kiri[0], buffer_target_goal.goal_kiri[1],
                buffer_target_goal.goal_kanan[0], buffer_target_goal.goal_kanan[1]);

            // float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(460, 1200);

            has_ball_state.value = 30;

            counter_menggiring_sebelum_tendang = 0;

            break;
        }

        break;
    }

    case 30: {
        /**
         * MENENTUKAN MOTION ATAU TOLERANSI UNTUK ARAH KE GAWANG
         * TOLERANSI -> ROBOT SUDAH MENGHADAP DI ANTARA GAWANG MIN DAN MAX
         * MOTION -> GERAK MENGHADAP KE TARGET SHOT
         */
        stm_data->global_kicker_position = motion->AutomaticKickerPos(40);

        buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();

        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(350, 1200) : motion->RobotAngletoPoint(455, 1200);
        float target_sudut_min = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(450, 1200);
        float target_sudut_max = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(370, 1200) : motion->RobotAngletoPoint(460, 1200);
        float delta_saya_ke_minimum = deltaAngle(final_pose->theta, target_sudut_min);
        float delta_saya_ke_maksimum = deltaAngle(final_pose->theta, target_sudut_max);
        float delta_minimum_ke_maksimum = deltaAngle(target_sudut_min, target_sudut_max);

        // TOLERANSI
        // ROBOT SUDAH MENGHADAP DI AREA GAWANG MINIMUM MAKSIMUM (TANPA GERAK)
        if (buffer_target_goal.status_gawang_kosong && delta_saya_ke_minimum < delta_minimum_ke_maksimum && delta_saya_ke_maksimum < delta_minimum_ke_maksimum) {
            LogWithColor("red", "SHOT PAKAI TOLERANSI BARU %.2f %.2f || target %.2f || saya %.2f\n",
                delta_saya_ke_minimum,
                delta_saya_ke_maksimum,
                target_sudut_shot,
                final_pose->theta);
            has_ball_state.value = 40;
        }

        // OUTPUT MOTION
        // ROBOT TIDAK MENGHADAP DI AREA GAWANG MINIMUM MAKSIMUM (NUNGGU GERAK SELESAI)
        // if (motion->pivotMotion(target_sudut_shot, 120))
        // {
        //     LogWithColor("red", "SHOT PAKAI TOLERANSI BIASA %.2f %.2f || target %.2f || saya %.2f\n",
        //                  delta_saya_ke_minimum,
        //                  delta_saya_ke_maksimum,
        //                  target_sudut_shot,
        //                  final_pose->theta);
        //     has_ball_state.value = 40;
        // }

        float kecepatan_translasi = 100;
        float vx_lapangan = kecepatan_translasi * cosf(target_sudut_shot DEG2RAD);
        float vy_lapangan = kecepatan_translasi * sinf(target_sudut_shot DEG2RAD);

        float vx_local = vx_lapangan * sinf(final_pose->theta DEG2RAD) - vy_lapangan * cosf(final_pose->theta DEG2RAD);
        float vy_local = vx_lapangan * cosf(final_pose->theta DEG2RAD) + vy_lapangan * sinf(final_pose->theta DEG2RAD);

        // COBA SAFETY
        if (final_pose->y > 940) {
            vx_local = 0;
            vy_local = 0;
        }

        if (bs_data->style == 'B') {
            /**
             * ! =========================================
             * ! STYLE B
             * ! TARGET GIRING -> TARGET FINAL POSITIONING
             * ! =========================================
             */
            vx_local = 0;
            vy_local = 0;
        }

        if (motion->hadapGawangVision(vx_local, vy_local, 120)) {
            LogWithColor("red", "SHOT PAKAI TOLERANSI BIASA %.2f %.2f || target %.2f || saya %.2f\n",
                delta_saya_ke_minimum,
                delta_saya_ke_maksimum,
                target_sudut_shot,
                final_pose->theta);
            kecepatan_translasi_maju = 100;
            has_ball_state.value = 40;
            prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
        }
        // if (motion->motionAngleControlPivot(vx_local * 0.3, vy_local, target_sudut_shot, 120)) {
        //     LogWithColor("red", "SHOT PAKAI TOLERANSI BIASA %.2f %.2f || target %.2f || saya %.2f\n",
        //         delta_saya_ke_minimum,
        //         delta_saya_ke_maksimum,
        //         target_sudut_shot,
        //         final_pose->theta);
        //     has_ball_state.value = 40;
        // }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_sebelum_gerak_maju, pos_y_sebelum_gerak_maju) > 50) {
            LogWithColor("red", "SHOT KARENA GERAK TERLALU MAJU %.2f %.2f || target %.2f || saya %.2f\n",
                delta_saya_ke_minimum,
                delta_saya_ke_maksimum,
                target_sudut_shot,
                final_pose->theta);
            kecepatan_translasi_maju = 100;
            has_ball_state.value = 40;
            prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
        }

        counter_menggiring_sebelum_tendang = 0;

        break;
    }
    case 40: {
        /**
         * TENDANG GOAL DENGAN MENGARAHKAN KE 350 ATAU 435 (SEBENTAR)
         */
        stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(350, 1200) : motion->RobotAngletoPoint(450, 1200);

        kecepatan_translasi_maju -= 0.5;

        float vx_lapangan = kecepatan_translasi_maju * cosf(target_sudut_shot DEG2RAD);
        float vy_lapangan = kecepatan_translasi_maju * sinf(target_sudut_shot DEG2RAD);

        float vx_local = vx_lapangan * sinf(final_pose->theta DEG2RAD) - vy_lapangan * cosf(final_pose->theta DEG2RAD);
        float vy_local = vx_lapangan * cosf(final_pose->theta DEG2RAD) + vy_lapangan * sinf(final_pose->theta DEG2RAD);

        if (final_pose->y > 950) {
            vx_local = 0;
            vy_local = 0;
        }

        // motion->motionAngleControl(vx_local, vy_local, target_sudut_shot, 20);

        if (*lock_goal_kiri_atau_kanan == prev_lock_goal_kiri_atau_kanan) {
            motion->hadapGawangVision(vx_local, vy_local, 160);
        } else {
            motion->motionAngleControl(vx_local, vy_local, target_sudut_shot, 60);
        }

        printf("TENDANG: %.2f %d %d\n", deltaAngle(final_pose->theta, target_sudut_shot), prev_lock_goal_kiri_atau_kanan, *lock_goal_kiri_atau_kanan);

        // motion->motionToPoint(final_pose->x, final_pose->y, target_sudut_shot, 0, 20);

        if (counter_menggiring_sebelum_tendang++ < 25) {
            break;
        }

        // SPAM TENDANG
        if (ball->is_caught > 0) {
            //=================================
            //        TENDANGAN GAWANG
            //=================================
            kickerControl(stm_data, NO_AIM, 30);
        }
        // else {
        //     has_ball_state.value = 50;
        //     has_ball_state.resetUptimeTimeout();
        // }

        break;
    }
    case 50:
        // motion->manualMotion(0, 0, 0);
        break;
    }

    prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
}
void Attacker::FriendHasBall() const
{
    static MachineState frined_has_ball_state;
    frined_has_ball_state.reentry(0, 5);

    static float buffer_x = 400, buffer_y = 600, target_x = 400, target_y = 600;

    float kecepatan_memindah_target = 5;

    float dx = target_x - buffer_x;
    float dy = target_y - buffer_y;
    float r = fminf(kecepatan_memindah_target, sqrtf(dx * dx + dy * dy));
    float a = atan2f(dy, dx);

    buffer_x += r * cosf(a);
    buffer_y += r * sinf(a);

    // if (bs_data->style != 'E') {
    //     if (shared_robot_data[bs_data->n_robot_def_1]->pose.x > FIELD_X_1_2) {
    //         target_x = FIELD_X_0 + 100;
    //         target_y = fmaxf(FIELD_Y_1_2 - 100, shared_robot_data[bs_data->n_robot_def_1]->pose.y);
    //     } else {
    //         target_x = FIELD_X_1 - 100;
    //         target_y = fmaxf(FIELD_Y_1_2 - 100, shared_robot_data[bs_data->n_robot_def_1]->pose.y);
    //     }
    // } else {
    //     target_x = *titik_final_positioning[0];
    //     target_y = *titik_final_positioning[1];
    // }

    if (bs_data->style != 'E') {
        if (final_pose->x < shared_robot_data[bs_data->n_robot_def_1]->pose.x) {
            target_x = FIELD_X_0 + 100;
            target_y = fmaxf(FIELD_Y_1_2 - 100, shared_robot_data[bs_data->n_robot_def_1]->pose.y);
        } else {
            target_x = FIELD_X_1 - 100;
            target_y = fmaxf(FIELD_Y_1_2 - 100, shared_robot_data[bs_data->n_robot_def_1]->pose.y);
        }
    } else {
        target_x = *titik_final_positioning[0];
        target_y = *titik_final_positioning[1];
    }

    LogWithColor("magenta", "FRIEND HAS BALL %.2f %.2f || %.2f %.2f\n", buffer_x, buffer_y, target_x, target_y);

    motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoBall(), 200, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
}
void Attacker::EnemyHasBall() const
{
    static MachineState enemy_has_ball_state;
    enemy_has_ball_state.reentry(0, 0.04);

    static float jarak_saya_ke_lawan;
    static obstacle_checking_t lawan;
    static float sudut_hadap_lawan;
    static float buffer_lawan[2];
    static float sudut_gawang_ke_bola = 90;

    static uint16_t counter_menghadap_depan_lawan = 0;

    lawan = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance, NORMAL_OBSTACLE);
    jarak_saya_ke_lawan = motion->RobotDist2Point(lawan.pos_x, lawan.pos_y);

    LogWithColor("yellow", "ENEMY HAS BALL %d ENEMY [Pos: %.2f %.2f Dist: %.2f] BALL [Dist: %.2f]\n",
        enemy_has_ball_state.value, lawan.pos_x, lawan.pos_y,
        jarak_saya_ke_lawan, ball->distance);

    switch (enemy_has_ball_state.value) {
    case 0:
        counter_menghadap_depan_lawan = 0;
        if (bs_data->style == 'C' || bs_data->style == 'M') {
            enemy_has_ball_state.value = 20;
            break;
        }

        sudut_hadap_lawan = atan2(ball->pos.y - lawan.pos_y, ball->pos.x - lawan.pos_x) RAD2DEG;

        if (jarak_saya_ke_lawan < ball->distance) {
            enemy_has_ball_state.value = 10;
            enemy_has_ball_state.resetUptimeTimeout();
        } else {
            enemy_has_ball_state.value = 0;
            // force_dribble_control = 1;
            motion->catchingBall(100, 90);
        }
        break;

    case 10:
        sudut_hadap_lawan = atan2(ball->pos.y - lawan.pos_y, ball->pos.x - lawan.pos_x) RAD2DEG;

        if (motion->motionAroundBall(sudut_hadap_lawan - 180, motion->RobotAngletoBall(), 50, 160, 100))
            enemy_has_ball_state.value = 0;

        if (jarak_saya_ke_lawan > ball->distance)
            counter_menghadap_depan_lawan++;

        if (counter_menghadap_depan_lawan >= 5)
            enemy_has_ball_state.value = 0;

        enemy_has_ball_state.timeout(0, 3);
        break;

    case 20:
        sudut_gawang_ke_bola = atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG;

        motion->motionAroundBall(sudut_gawang_ke_bola, sudut_gawang_ke_bola, 80, 150, 100);
        break;
    }
}
void Attacker::WildBall() const
{
    static float sudut_robot_ke_bola;
    static float sudut_gawang_ke_bola;
    static float ball_magnitude;

    static MachineState wild_ball_state;
    wild_ball_state.reentry(0, 0.05);

    ball_magnitude = pythagoras(ball->vel.x, ball->vel.y, 0, 0);

    // Intercept 2024
    static float sudut_arah_bola;
    static float kecepatan_bola;
    static uint8_t counter_bola_bergerak = 0;

    sudut_arah_bola = atan2(ball->vel.y, ball->vel.x) RAD2DEG;
    kecepatan_bola = pythagoras(ball->vel.x, ball->vel.y, 0, 0);

    LogWithColor("green", "WILDBALL %d theta %.2f || %.2f || %.2f %.2f\n", wild_ball_state.value, final_pose->theta, ball_magnitude, kecepatan_bola, deltaAngle(sudut_arah_bola - 180, motion->RobotAngletoBall()));

    switch (wild_ball_state.value) {
    case 0:
        /**
         * BLOCK ENEMY -> MENGHALANGI OBS DENGAN POS OBS
         * BLOCK ENEMY -> MENGHALANGI OBS DENGAN POS TENGAH LAPANGAN DENGAN RADIUS 150
         * BLOCK ENEMY -> CATCHING BALL
         */
        BlockEnemy();

        /**
         * 100 < BALL DISTANCE < 135
         * FINAL POSE Y > BALL POS Y
         * BALL POS X TIDAK TERLALU PINGGIR
         * BALL POS Y > 300
         */
        // if (ball->distance < 135 && ball->distance > 100 && final_pose->y > ball->pos.y && ball->pos.x > FIELD_X_0 + 30 && ball->pos.y < FIELD_X_1 - 30 && ball->pos.y > FIELD_Y_1_4 && ball_magnitude > 50) {
        //     wild_ball_state.value = 1;
        //     wild_ball_state.resetUptimeTimeout();
        // }

        // Intercept 2024
        if (kecepatan_bola > 20) {
            counter_bola_bergerak++;
        } else if (kecepatan_bola < 20) {
            counter_bola_bergerak = 0;
        }

        // Intercept 2024
        if (bs_data->n_active_robot > 1 && ball->pos.y > FIELD_Y_1_4 && counter_bola_bergerak > 20 && ball->pos.y > FIELD_X_1_5 && ball->pos.x > FIELD_X_0 + 30 && ball->pos.y < FIELD_X_1 - 30 && deltaAngle(sudut_arah_bola - 180, motion->RobotAngletoBall()) > 30 && ball->distance > 100) {
            wild_ball_state.value = 10;
            wild_ball_state.resetUptimeTimeout();
        } else if (bs_data->n_active_robot <= 1 && counter_bola_bergerak > 20 && ball->pos.y > FIELD_X_1_5 && ball->pos.x > FIELD_X_0 + 30 && ball->pos.y < FIELD_X_1 - 30 && deltaAngle(sudut_arah_bola - 180, motion->RobotAngletoBall()) > 30 && ball->distance > 100) {
            wild_ball_state.value = 10;
            wild_ball_state.resetUptimeTimeout();
        }

        break;

    case 1:
        /**
         * RESET STATE KE 0
         */
        sudut_gawang_ke_bola = atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG;
        sudut_robot_ke_bola = atan2(ball->pos.y - final_pose->y, ball->pos.x - final_pose->x) RAD2DEG;

        if (motion->motionAroundBall(sudut_gawang_ke_bola, sudut_robot_ke_bola, 75, 210, 100)) {
            wild_ball_state.value = 0;
        }

        // printf("Sudut: %.2f %.2f -> %.2f\n", sudut_gawang_ke_bola, sudut_robot_ke_bola, deltaAngle(sudut_gawang_ke_bola, sudut_robot_ke_bola));
        if (deltaAngle(sudut_gawang_ke_bola, sudut_robot_ke_bola) < 10) {
            wild_ball_state.value = 0;
        }

        if (final_pose->y < FIELD_Y_1_4 - 10) {
            wild_ball_state.value = 0;
        }

        wild_ball_state.timeout(0, 5);
        break;

    case 10: {
        // Intercept 2024
        // if (motion->motionAroundBall(sudut_arah_bola - 180, motion->RobotAngletoBall(), 100, 210, 120))
        // {
        //     wild_ball_state.value = 0;
        // }

        float pos_x_intercept = ball->pos.x + fminf(fmaxf(100, kecepatan_bola * 2), 200) * cosf(sudut_arah_bola DEG2RAD);
        float pos_y_intercept = ball->pos.y + fminf(fmaxf(100, kecepatan_bola * 2), 200) * sinf(sudut_arah_bola DEG2RAD);

        if (motion->motionToPoint(pos_x_intercept, pos_y_intercept, ball->angle, 210, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            wild_ball_state.value = 0;
        }

        // Ketika robot sudah meng-intercepet
        if (deltaAngle(sudut_arah_bola - 180, motion->RobotAngletoBall()) < 120) {
            wild_ball_state.value = 0;
        }

        // Safety ketika robot terlalu pinggir
        if (final_pose->x < FIELD_X_0 + 100 || final_pose->x > FIELD_X_1 - 100 || final_pose->y < FIELD_Y_0 + 100 || final_pose->y > FIELD_Y_1 - 100) {
            wild_ball_state.value = 0;
        }

        // Safety ketika robot terlalu pinggir
        if (pos_x_intercept < FIELD_X_0 + 100 || pos_x_intercept > FIELD_X_1 - 100 || pos_y_intercept < FIELD_Y_0 + 100 || pos_y_intercept > FIELD_Y_1 - 100) {
            wild_ball_state.value = 0;
        }

        wild_ball_state.timeout(0, 5);
        break;
    }
    }
}
void Attacker::Roaming(int default_state) const
{
    static MachineState roaming_state;

    roaming_state.reentry(default_state, 2);

    static float target_sudut_patroli = 0;

    target_sudut_patroli = atan2(final_pose->y - FIELD_Y_1_2, final_pose->x - FIELD_X_1_2) RAD2DEG - 180;

    if (*enemy_locked) {
        *enemy_locked_obs = motion->obstacleChecking(motion->RobotAngletoPoint(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y), 40,
            motion->RobotDist2Point(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y) + 100, NORMAL_OBSTACLE);
    }

    if (motion->RobotDist2Point(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y) > 275 || final_pose->y < FIELD_Y_1_4) {
        *enemy_locked = 0;
    }

    if (bs_data->style == 'C' || bs_data->style == 'M') {
        roaming_state.value = 100;
    }

    if (*enemy_locked == 1) {
        roaming_state.value = 6;
    }
    LogWithColor("cyan", "ROAMING %d ENEMY [Locked: %d dist: %.2f]\n", roaming_state.value, *enemy_locked, motion->RobotDist2Point(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y));

    switch (roaming_state.value) {
    case 0:
        // MENENTUKAN STATE
        roaming_state.value = default_state;
        break;

        //==================================================================================

    case 10:
        /**
         * STATE SETELAH WILD BALL
         */
        roaming_state.resetUptimeTimeout();
        if (*bola_terakhir_sebelum_hilang[0] == 0 && *bola_terakhir_sebelum_hilang[1] == 0)
            roaming_state.value = 2;
        else
            roaming_state.value = 1;
        break;
    case 1:
        /**
         * MENUJU LOKASI TERAKHIR BOLA HILANG
         */
        LogWithColor("green", "TIME OUT ROAMING STATE 1\n");

        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
            motion->motionToPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1], motion->RobotAngletoPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1]), 10, 10, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        } else {
            motion->motionToPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1], motion->RobotAngletoPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1]), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        }
        roaming_state.timeout(2, 2);
        break;

        //==================================================================================

    case 2:
        if (bs_data->n_active_robot > 1) {
            if (*bola_terakhir_sebelum_hilang[0] > FIELD_X_1_2 && *bola_terakhir_sebelum_hilang[1] > FIELD_Y_1_2) {
                // KANAN ATAS
                roaming_state.value = 4;
            } else if (*bola_terakhir_sebelum_hilang[0] < FIELD_X_1_2 && *bola_terakhir_sebelum_hilang[1] > FIELD_Y_1_2) {
                // KIRI ATAS
                roaming_state.value = 5;
            } else if (*bola_terakhir_sebelum_hilang[0] < FIELD_X_1_2 && *bola_terakhir_sebelum_hilang[1] < FIELD_Y_1_2) {
                // KIRI BAWAH
                roaming_state.value = 21;
            } else {
                // KANAN BAWAH
                roaming_state.value = 3;
            }
            break;
        } else {
            if (*bola_terakhir_sebelum_hilang[1] > FIELD_Y_1_2) {
                roaming_state.value = 22;
            } else {
                roaming_state.value = 33;
            }
            break;
        }

        //==================================================================================
        /**
         * BERGERAK KOTAK MENGITARI TENGAH LAPANGAN
         * 21 -> KIRI BAWAH
         * 3 -> KANAN BAWAH
         * 4 -> KANAN ATAS
         * 5 -> KIRI ATAS
         */

    case 21:
        if (motion->motionToPoint(FIELD_X_1_2 - 0, FIELD_Y_1_2 - 0 - 160, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 3;
        }

        if (motion->RobotDist2Point(FIELD_X_1_2 - 0, FIELD_Y_1_2 - 0 - 160) < 200) {
            roaming_state.value = 3;
        }
        break;
    case 3:
        if (motion->motionToPoint(FIELD_X_1_2 + 0, FIELD_Y_1_2 - 0 - 90, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 4;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2 + 0, FIELD_Y_1_2 - 0 - 90) < 200) {
            roaming_state.value = 4;
        }
        break;
    case 4:
        if (motion->motionToPoint(FIELD_X_1_2 + 0, FIELD_Y_1_2 + 0 + 20, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 5;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2 + 0, FIELD_Y_1_2 + 0 + 20) < 200) {
            roaming_state.value = 5;
        }
        break;
    case 5:
        if (motion->motionToPoint(FIELD_X_1_2 - 0, FIELD_Y_1_2 + 0 + 70, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 21;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2 - 0, FIELD_Y_1_2 + 0 + 70) < 200) {
            roaming_state.value = 21;
        }
        break;

        //==================================================================================

    case 22:
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_2 + 190 + 100, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 33;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1_2 + 190 + 100) < 200) {
            roaming_state.value = 33;
        }
        break;
    case 33:
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_2 - 190 - 100, 90, 130, 75, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE)) {
            roaming_state.value = 22;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1_2 - 190 - 100) < 200) {
            roaming_state.value = 22;
        }
        break;

        //==================================================================================

    case 6:
        /**
         * LOCK ENEMY
         * NEXT STATE -> 2 (PERSIAPAN GERAK KOTAK)
         */
        LogWithColor("red", "LOCK ENEMY\n");
        if (*enemy_locked == 0) {
            roaming_state.value = 2;
            break;
        }

        motion->motionAroundPointAngle(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y,
            atan2(enemy_locked_obs->pos_y - 0, enemy_locked_obs->pos_x - 400) RAD2DEG,
            atan2(enemy_locked_obs->pos_y - final_pose->y, enemy_locked_obs->pos_x - final_pose->x) RAD2DEG,
            100, 160, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        break;

        //==================================================================================

    case 100:
        /**
         * ! =========================================
         * ! STYLE C
         * ! JAGA MENUNGGU BOLA
         * ! =========================================
         */
        if (bs_data->n_active_robot > 1) {
            motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_2 - 100, 90, 120, 90); // jaga agak depan
        } else {
            motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 120, 90); // jaga dibelakang
        }
        break;
    }
}
void Attacker::EnemyLock(float enemy_x, float enemy_y) const
{
    static MachineState enemy_lock_state;

    enemy_lock_state.reentry(0, 2);

    switch (enemy_lock_state.value) {
    case 0:
        /**
         * KEJAR MUSUH DENGAN RADIUS 80
         */
        motion->motionAroundPointAngle(enemy_x, enemy_y, atan2(enemy_y - 0, enemy_x - 400) RAD2DEG, atan2(enemy_y - 0, enemy_x - 400) RAD2DEG, 80, 150, 75);
        break;
    }
}
void Attacker::Penalty() const { }
void Attacker::Park() const { }
void Attacker::BlockEnemy() const
{
    static obstacle_checking_t obstacle;
    // CEK OBSTACLE KE ARAH GAWANG HOME
    obstacle = motion->obstacleChecking(-90, 180, ball->distance, NORMAL_OBSTACLE);

    float jarak_obstacle_ke_bola = pythagoras(obstacle.pos_x, obstacle.pos_y, ball->pos.x, ball->pos.y);

    if (ball->pos.y < FIELD_Y_1_4 && bs_data->n_active_robot > 1) {
        motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, atan2f(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG, motion->RobotAngletoBall() - 180, 150, 80, 60, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | INVERT);
    } else {
        obstacle_checking_t obs_depan = motion->obstacleChecking(final_pose->theta, 25, 120, NORMAL_OBSTACLE);

        // Ketika bola dipinggir
        if ((ball->pos.x < FIELD_X_0 + 70 && final_pose->x < FIELD_X_0 + 170) || (ball->pos.x > FIELD_X_1 - 70 && final_pose->x > FIELD_X_1 - 170)) {
            // Ketika sudah menghadap lapangan
            if (deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_3_4)) < 30) {
                if (obs_depan.status) {
                    LogWithColor("blue", "KEJAR LAMBAT habis putari\n");
                    motion->catchingBall(160, 150);
                } else {
                    LogWithColor("red", "KEJAR CEPAT habis putari\n");
                    motion->catchingBall(160, 150, LANGSUNG_AMBIL);
                }
            } else {
                // HARUSNYA DI STOP REFBOX
                if (ball->pos.x < FIELD_X_0 + 40 || ball->pos.x > FIELD_X_1 - 40) {
                    LogWithColor("blue", "HARUSNYA STOP\n");
                    motion->catchingBall(20, 50);
                } else {
                    LogWithColor("red", "Putari bola hadap lapangan\n");
                    motion->motionAroundBall(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_3_4), motion->RobotAngletoBall(), 30, 150, 90, NORMAL_OBSTACLE);
                }
            }
        }
        // Ketika bola didepan atau belakang
        else if ((ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 150) || (ball->pos.y > FIELD_Y_1 - 70 && final_pose->y > FIELD_Y_1 - 150)) {
            LogWithColor("cyan", "KEJAR PELAN SEKALI\n");
            motion->catchingBall(20, 50);
        } else {
            if (obs_depan.status) {
                LogWithColor("blue", "KEJAR LAMBAT\n");
                motion->catchingBall(160, 150);
            } else {
                if (ball->distance < 300) {
                    LogWithColor("magenta", "KEJAR CEPAT\n");
                    motion->catchingBall(200, 180, LANGSUNG_AMBIL);
                } else {
                    LogWithColor("red", "KEJAR CEPAT SEKALI\n");
                    motion->catchingBall(200, 180, LANGSUNG_AMBIL);
                }
            }
        }
    }
}

void Attacker::StandBy(int default_r, float x_arching) const { }
void Attacker::Intercept() const { }

void Attacker::preparationGlobal() const
{
    // bs_data->obs_bs_regional[0] = 1;
    // bs_data->obs_bs_regional[1] = 7;
    // bs_data->obs_bs_regional[2] = 2;

    game_sub_status_prep->reentry(0, 0.05);
    printf("Attacker::preparationGlobal() %d || %d %d\n", game_sub_status_prep->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition);

    // printf("obs : %d %d %d\n", bs_data->obs_bs_regional[0], bs_data->obs_bs_regional[1], bs_data->obs_bs_regional[2]);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        if (pythagoras(final_pose->x, final_pose->y, BASE_1.x, BASE_1.y) > 20) {
            game_sub_status_prep->value = 1;
        }
        motion->manualMotion(0, 0, 0);
        break;
    case 1:
        if (final_pose->y < 250) {
            game_sub_status_prep->value = 44;
            break;
        }
        if (final_pose->y - BASE_1.y > 10) {
            game_sub_status_prep->value = 11;
        } else if (final_pose->y - BASE_1.y < -10) {
            game_sub_status_prep->value = 22;
        } else {
            game_sub_status_prep->value = 33;
        }
        break;
    case 11:
        if (motion->motionToPoint(BASE_1.x, BASE_1.y - 10, BASE_1.theta, 285, 250)) {
            game_sub_status_prep->value = 0;
        }

        // if (motion->motionToPoint(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
        //     game_sub_status_prep->value = 0;
        // }

        if (pythagoras(final_pose->x, final_pose->y, BASE_1.x, BASE_1.y - 10) < 20) {
            game_sub_status_prep->value = 0;
        }
        break;
    case 22:
        if (motion->motionToPoint(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            game_sub_status_prep->value = 0;
        }

        // if (motion->motionToPoint(BASE_1.x+5, BASE_1.y, BASE_1.theta, 300, 250)) {
        //     game_sub_status_prep->value = 0;
        // }

        if (pythagoras(final_pose->x, final_pose->y, BASE_1.x, BASE_1.y) < 20) {
            game_sub_status_prep->value = 0;
        }
        break;
    case 33:
        if (motion->motionToPoint(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            game_sub_status_prep->value = 0;
        }

        // if (motion->motionToPoint(BASE_1.x+5 + 10, BASE_1.y, BASE_1.theta, 300, 250)) {
        //     game_sub_status_prep->value = 0;
        // }

        if (pythagoras(final_pose->x, final_pose->y, BASE_1.x, BASE_1.y) < 20) {
            game_sub_status_prep->value = 0;
        }
        break;
    case 44:
        if (motion->motionToPoint(BASE_1.x, BASE_1.y - 10, BASE_1.theta, 200, 250)) {
            game_sub_status_prep->value = 0;
        }

        // if (motion->motionToPoint(BASE_1.x+5 + 10, BASE_1.y, BASE_1.theta, 300, 250)) {
        //     game_sub_status_prep->value = 0;
        // }

        if (pythagoras(final_pose->x, final_pose->y, BASE_1.x, BASE_1.y - 10) < 20) {
            game_sub_status_prep->value = 0;
        }
        break;
    }
}

void Attacker::gameStyleA() const
{
    static float pos_x_recv_ball = 0;
    static float pos_y_recv_ball = 0;
    static float pos_x_kick_ball = 0;
    static float pos_y_kick_ball = 0;
    static float error_th = 0;

    static float r2_recv_x = 0;
    static float r2_recv_y = 0;
    static float r2_pass_x = 0;
    static float r2_pass_y = 0;

    static float r3_pass_x = 0;
    static float r3_pass_y = 0;
    static float r3_recv_x = 0;
    static float r3_recv_y = 0;

    static float pos_x_shoot = 0;
    static float pos_y_shoot = 0;

    static float pos_x_buffer = 0;
    static float pos_y_buffer = 0;

    float pos_x_friend = shared_robot_data[3]->pose.x;
    float pos_y_friend = shared_robot_data[3]->pose.y;

    r3_recv_x = pos_x_friend;
    r3_recv_y = pos_y_friend;

    uint8_t obs_keeper = bs_data->obs_bs_regional[0];
    uint8_t obs_tengah = bs_data->obs_bs_regional[1];
    uint8_t obs_samping = bs_data->obs_bs_regional[2];

    LogWithColor("green", "Attacker::gameStyleA() %d || %d %d || %d || %.2f [%.2f %.2f]\n",
        game_sub_status->value, shared_robot_data[2]->robot_condition,
        shared_robot_data[3]->robot_condition, ball->status, error_th,
        r3_recv_x, r3_recv_y);

    if (pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y) < 100 && pos_x_recv_ball != 0 && pos_y_recv_ball != 0) {
        ROS_ERROR("STYLE A BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y));
    }

    if (pythagoras(pos_x_kick_ball, pos_y_kick_ball, r2_recv_x, r2_recv_y) < 100 && pos_x_kick_ball != 0 && pos_y_kick_ball != 0) {
        ROS_WARN("STYLE A BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_kick_ball, pos_y_kick_ball, r2_recv_x, r2_recv_y));
    }

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    /*
     * ===============================================================
     *                       ROBOT 1 AMBIL
     * ===============================================================
     */
    case 0:
        stm_data->global_kicker_position = 900;
        *robot_condition = 0;
        // printf("AMBIL BOLA\n");
        // if (motion->ruckigToPoint3(480, 650, -170, 250, 200)) {
        //     game_sub_status->value = 101;
        // }

        // if (pythagoras(final_pose->x, final_pose->y, 480, 650) < 150) {
        //     game_sub_status->value = 101;
        // }

        // if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 150) {
        //     game_sub_status->value = 101;
        // }
        game_sub_status->value = 123;
        break;
    case 123:
        if (motion->motionToPoint(480, 600, 180, 300, 90)) {
            game_sub_status->value = 124;
        }
        if (motion->RobotDist2Point(480, 600) < 120) {
            game_sub_status->value = 124;
        }
        break;
    case 124:
        if (motion->motionToPoint(FIELD_X_1_2 + 30, FIELD_Y_1_2 + 50, -180, 150, 90)) {
            game_sub_status->value = 125;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1_2) < 100) {
            game_sub_status->value = 125;
        }
        break;
    case 125:
        if (motion->motionAroundBall(-180, ball->angle, 80, 120, 90)) {
            game_sub_status->value = 1;
        }
        {
            float error_robot_hadap = -180 - final_pose->theta;
            while (error_robot_hadap > 180)
                error_robot_hadap -= 360;
            while (error_robot_hadap < -180)
                error_robot_hadap += 360;

            if (fabs(error_robot_hadap) < 8) {
                game_sub_status->value = 1;
            }
        }
        break;
    case 101:
        *robot_condition = 0;
        printf("AMBIL BOLA\n");
        if (motion->ruckigToPoint3(ball->pos.x + 100, ball->pos.y, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 100, 200)) {
            game_sub_status->value = 1;
        }
        break;
    case 1:
        // printf("TEST %.2f %.2f\n", ball->pos.y, final_pose->y);
        // if (ball->pos.y > final_pose->y) {
        //     motion->motionToPoint(final_pose->x, ball->pos.y + 40, ball->angle, 100, 90);
        //     break;
        // }
        printf("CATCHING BALL\n");
        if (motion->catchingBall(80, 19)) {
            game_sub_status->value = 2; // 2  2
        }
        break;
    case 2:
        if (motion->pivotMotion(angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 180)) {
            game_sub_status->value = 3;
        }
        error_th = angleToTarget(pos_x_friend, pos_y_friend, *final_pose) - final_pose->theta;

        if (error_th > 180)
            error_th -= 360;
        if (error_th < -180)
            error_th += 360;

        if (fabs(error_th) < 3) {
            game_sub_status->value = 3;
        }
        break;
    case 3:
        printf("TENDANG BOLA 1\n");
        // motion->manualMotion(0, 0, 0);
        motion->motionAngleControl(0, 0, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 20);

        if (ball->is_caught == 0) {
            game_sub_status->value = 33;
            *robot_condition = 11;
        } else {
            kickerControl(stm_data, NO_AIM, 8, 900);
        }
        break;
    /*
     * ===============================================================
     *                       ROBOT 2 UMPAN 1
     * ===============================================================
     */
    case 33:
        r2_recv_x = 720;
        r2_recv_y = 530;

        r3_pass_x = 50;
        r3_pass_y = 525;
        game_sub_status->value = 4;
        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 1\n");
        if (motion->motionToPoint(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            game_sub_status->value = 44;
        }

        if (pythagoras(final_pose->x, final_pose->y, r2_recv_x, r2_recv_y) < 100) {
            game_sub_status->value = 44;
        }

        if (shared_robot_data[3]->robot_condition == 11) {
            buzzerControl(stm_data, 10, 10);
            game_sub_status->value = 5;
            *robot_condition = 0;
        }

        break;
    case 44:
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            buzzerControl(stm_data, 10, 10);
            game_sub_status->value = 5;
            *robot_condition = 0;
        }
        break;
    case 5:
        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint3(final_pose->x, final_pose->y, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
                game_sub_status->value = 55;
                pos_x_recv_ball = final_pose->x;
                pos_y_recv_ball = final_pose->y;
            }
        }

        break;
    /*
     * ===============================================================
     *                        ROBOT 1 UMPAN 1
     * ===============================================================
     */
    case 55:
        *robot_condition = 0;
        //? SEBELUM 530
        if (bs_data->obs_bs_regional[2] == 1 || bs_data->obs_bs_regional[2] == 4) {
            if (bs_data->obs_bs_regional[1] == 7) {
                r2_pass_x = 770;
                r2_pass_y = 260; //* GIRING JAUH  (+270)
            } else if (bs_data->obs_bs_regional[1] == 8) {
                r2_pass_x = 770;
                r2_pass_y = 345; //* GIRING JAUH  (+185)
            }
            game_sub_status->value = 665; // DENGAN SUDUT
            break;
        } else {
            r2_pass_x = 730;
            r2_pass_y = 360; //* GIRING DEKAT (+170)
        }
        game_sub_status->value = 64;
        break;
    case 64:
        *robot_condition = 0;
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, 180, 100, 200)) {
            game_sub_status->value = 66;
        }
        if (pythagoras(final_pose->x, final_pose->y, r2_pass_x, r2_pass_y) < 150) {
            game_sub_status->value = 66;
        }
        break;
    case 665:
        if (final_pose->x > 650) {
            printf("TRESHOLD SAFETY 1 %.2f\n", final_pose->x);
            game_sub_status->value = 65;
            break;
        }

        if (bs_data->obs_bs_regional[2] != 1) {
            game_sub_status->value = 65;
            break;
        }

        if (motion->motionToPoint(r2_pass_x, final_pose->y, 160, 300, 250)) {
            game_sub_status->value = 65;
        }

        break;
    case 65:
        *robot_condition = 0;
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, 160, 100, 200)) {
            game_sub_status->value = 66;
        }
        if (pythagoras(final_pose->x, final_pose->y, r2_pass_x, r2_pass_y) < 150) {
            game_sub_status->value = 66;
        }
        break;
    case 66:
        // *robot_condition = 11;
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(r3_recv_x, r3_recv_y, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
            game_sub_status->value = 67;
            printf("ATAS\n");
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_recv_ball, pos_y_recv_ball) > 50 && fabs(error_th) < 1) {
            game_sub_status->value = 67;
            printf("BAWAH\n");
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 67:
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 50);
        game_sub_status->timeout(7, 1);
        break;
    case 7:
        *robot_condition = 11;
        printf("TENDANG BOLA 2\n");
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 20);

        if (ball->is_caught == 0) {
            // *robot_condition = 11;
            game_sub_status->value = 77;
        } else {
            if (bs_data->obs_bs_regional[2] == 5 || bs_data->obs_bs_regional[2] == 6 || bs_data->obs_bs_regional[2] == 2 || bs_data->obs_bs_regional[2] == 3) {
                kickerControl(stm_data, THROTTLE_AIM, 10, 800);
            } else {
                if (obs_samping == 3 && obs_tengah == 8) {
                    kickerControl(stm_data, THROTTLE_AIM, 11, 800);
                } else if (bs_data->obs_bs_regional[2] == 1 || bs_data->obs_bs_regional[2] == 4) {
                    kickerControl(stm_data, THROTTLE_AIM, 8, 800);
                } else {
                    kickerControl(stm_data, THROTTLE_AIM, 8, 800);
                }
            }
            pos_x_kick_ball = final_pose->x;
            pos_y_kick_ball = final_pose->y;
        }
        break;
    /*
     * ===============================================================
     *                        ROBOT 2 UMPAN 2
     * ===============================================================
     */
    case 77:
        //* KEMUNGKINAN OBS SAMPING DI KIRI
        if (obs_samping == 5) {
            if (obs_tengah == 7) {
                r2_recv_x = 650;
                r2_recv_y = 190; //* AMAN (360) else (+170)
            } else if (obs_tengah == 8) {
                r2_recv_x = 630;
                r2_recv_y = 290; // BAHAYA (360) else (+70)

                r2_recv_y = pos_y_kick_ball - 70; // BAHAYA (350)
                if (r2_recv_y < 290)
                    r2_recv_y = 290;
            }
        } else if (obs_samping == 6) {
            if (obs_tengah == 8) {
                r2_recv_x = 630;
                r2_recv_y = 290; // BAHAYA (360) else (+70)

                r2_recv_y = pos_y_kick_ball - 70; // BAHAYA (350)
                if (r2_recv_y < 290)
                    r2_recv_y = 290;
            } else if (obs_tengah == 7) {
                r2_recv_x = 650;
                r2_recv_y = 190; //* AMAN (360) else (+170)
            }
        } else if (obs_samping == 4) {
            if (obs_tengah == 8) {
                r2_recv_x = 650;
                r2_recv_y = 230; //* AMAN (360) if (+130)
            } else if (obs_tengah == 7) {
                r2_recv_x = 650;
                r2_recv_y = 190; //* AMAN (350) if (+170)
            }
        }

        //* KEMUNGKINAN OBS SAMPING DI KANAN
        if (obs_samping == 2) {
            if (obs_tengah == 7) {
                r2_recv_x = 700;
                r2_recv_y = 230; //* AMAN (360) (+130)
            } else if (obs_tengah == 8) {
                r2_recv_x = 700;
                r2_recv_y = 170; //* AMAN (360) (+190)
            }
        } else if (obs_samping == 3) {
            if (obs_tengah == 7) {
                r2_recv_x = 600;
                r2_recv_y = pos_y_kick_ball - 90; // BAHAYA (350)
                if (r2_recv_y < 270)
                    r2_recv_y = 270;
            } else if (obs_tengah == 8) {
                r2_recv_x = 600;
                r2_recv_y = pos_y_kick_ball - 60; // BAHAYA (350)
                if (r2_recv_y < 270)
                    r2_recv_y = 270;
            }
        } else if (obs_samping == 1) {
            r2_recv_x = 630;
            r2_recv_y = 200; //* AMAN (260 KALO 7 (+60)) AMAN (345 KALO 8 (+245))

            r2_recv_y = pos_y_kick_ball - 90; // BAHAYA (350)

            if (obs_tengah == 7) {
                r2_recv_x = 670;
                if (r2_recv_y < 180)
                    r2_recv_y = 180;
            } else if (obs_tengah == 8) {
                r2_recv_x = 630;
                if (r2_recv_y < 240)
                    r2_recv_y = 240;
            }
        }

        game_sub_status->value = 8;
        break;
    case 8:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            game_sub_status->value = 9;
        }
        break;
    case 9:
        printf("TUNGGU TERIMA BOLA 2\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            buzzerControl(stm_data, 10, 10);
            game_sub_status->value = 91;
            *robot_condition = 0;
        }
        break;
    case 91:
        printf("TERIMA BOLA 2\n");

        if (ball->status == 0 && ball->is_caught == 0) {
            motion->manualMotion(0, 0, 0);
        } else {
            if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
                game_sub_status->value = 99;
            }
        }
        break;
    /*
     * ===============================================================
     *                        ROBOT 1 GOAL
     * ===============================================================
     */
    case 99:
        if (obs_keeper == 1) {
            pos_x_shoot = 360; // TEMBAK 1
            pos_y_shoot = 0;
        } else {
            pos_x_shoot = 470; // TEMBAK 2 ATAU 3
            pos_y_shoot = 0;
        }

        if (obs_keeper == 1 && obs_samping == 1) {
            r2_pass_x = 600;
        } else {
            r2_pass_x = final_pose->x;
        }

        if (final_pose->y < 200) {
            r2_pass_y = 230;
        } else {
            if (obs_samping == 3) {
                r2_pass_y = 290;
            } else {
                r2_pass_y = final_pose->y;
            }
        }

        printf("HALO %.2f %.2f\n", r2_pass_x, r2_pass_y);

        game_sub_status->value = 112;

        break;
    case 112:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->motionToPoint(r2_pass_x, r2_pass_y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 155)) {
            game_sub_status->value = 10;
        }
        if (fabs(error_th) < 3 && pythagoras(final_pose->x, final_pose->y, r2_pass_x, r2_pass_y) < 70) {
            game_sub_status->value = 10;
        }
        break;
    case 10:
        printf("PERSIAPAN TENDANG GOAL\n");
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 155)) {
            game_sub_status->value = 11;
        }

        if (bs_data->obs_bs_regional[0] == 1 || bs_data->obs_bs_regional[0] == 3) {
            if (fabs(error_th) < 1) {
                game_sub_status->value = 11;
            }
        } else {
            if (fabs(error_th) < 2) // 2.5
            {
                game_sub_status->value = 11;
            }
        }
        break;
    case 11:
        printf("TENDANG GOAL\n");
        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 0) {
            game_sub_status->value = 12;
        } else {
            if (bs_data->obs_bs_regional[1] == 8 && bs_data->obs_bs_regional[2] == 2) {
                kickerControl(stm_data, THROTTLE_AIM, 15); // DIPELANKAN SEMENTARA KETIKA LATIHAN
            } else {
                kickerControl(stm_data, THROTTLE_AIM, 15);
            }
        }
        break;
    /*
     * ===============================================================
     *                            END
     * ===============================================================
     */
    case 12:
        if (motion->ruckigToPoint3(BASE_1.x, final_pose->y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        if (final_pose->x > 650 || final_pose->y > 250) {
            game_sub_status->value = 13;
        }
        break;
    case 13:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 230)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Attacker::gameStyleB() const
{
    static float pos_x_recv_ball = 0;
    static float pos_y_recv_ball = 0;
    static float pos_x_kick_ball = 0;
    static float pos_y_kick_ball = 0;
    static float error_th = 0;

    static float r2_recv_x = 0;
    static float r2_recv_y = 0;
    static float r2_pass_x = 0;
    static float r2_pass_y = 0;

    static float r3_pass_x = 0;
    static float r3_pass_y = 0;
    static float r3_recv_x = 0;
    static float r3_recv_y = 0;

    static float pos_x_shoot = 0;
    static float pos_y_shoot = 0;

    float pos_x_friend = shared_robot_data[3]->pose.x;
    float pos_y_friend = shared_robot_data[3]->pose.y;

    r3_recv_x = pos_x_friend;
    r3_recv_y = pos_y_friend;

    uint8_t obs_keeper = bs_data->obs_bs_regional[0];
    uint8_t obs_tengah = bs_data->obs_bs_regional[1];
    uint8_t obs_samping = bs_data->obs_bs_regional[2];

    LogWithColor("magenta", "Attacker::gameStyleB() %d || %d %d || %d || %.2f [%.2f %.2f]\n",
        game_sub_status->value, shared_robot_data[2]->robot_condition,
        shared_robot_data[3]->robot_condition, ball->status, error_th,
        r3_recv_x, r3_recv_y);

    if (pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y) < 100 && pos_x_recv_ball != 0 && pos_y_recv_ball != 0) {
        ROS_ERROR("STYLE B BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y));
    }

    if (pythagoras(pos_x_kick_ball, pos_y_kick_ball, r2_recv_x, r2_recv_y) < 100 && pos_x_kick_ball != 0 && pos_y_kick_ball != 0) {
        ROS_WARN("STYLE B BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_kick_ball, pos_y_kick_ball, r2_recv_x, r2_recv_y));
    }

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    /*
     * ===============================================================
     *                       ROBOT 2 AMBIL
     * ===============================================================
     */
    case 0:
        stm_data->global_kicker_position = 900;
        // r2_recv_x = 680;
        // r2_recv_y = 495;
        r2_recv_x = 720; // tak ganti
        r2_recv_y = 550;

        *robot_condition = 0;
        printf("PERSIAPAN TERIMA BOLA 1\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(400, 600, *final_pose), 250, 190)) {
            game_sub_status->value = 10;
        }
        break;
    case 10:
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            buzzerControl(stm_data, 10, 10);
            game_sub_status->value = 1;
        }
        break;
    case 1:
        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint3(final_pose->x, final_pose->y, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
                game_sub_status->value = 11;
                pos_x_recv_ball = final_pose->x;
                pos_y_recv_ball = final_pose->y;
            }
        }
        break;
    /*
     * ===============================================================
     *                       ROBOT 1 UMPAN 1
     * ===============================================================
     */
    case 11:
        r2_pass_x = 600; // 600 asline
        r2_pass_y = 470; //! BAHAYA 550 (+80)

        r3_recv_x = 150;
        r3_recv_y = 510;
        game_sub_status->value = 22;
        break;
    case 22:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        r3_recv_y -= 38; // TAK UBAH

        error_th = angleToTarget(r3_recv_x, r3_recv_y, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
            game_sub_status->value = 222;
            printf("ATAS\n");
        }
        if (pythagoras(final_pose->x, final_pose->y, r2_pass_x, r2_pass_y) < 70 && fabs(error_th) < 1) {
            game_sub_status->value = 222;
            printf("BAWAH\n");
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 222:
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 20);
        printf("DIAM SEBENTAR\n");
        game_sub_status->timeout(3, 0.2);
        break;
    case 3:
        *robot_condition = 11;
        printf("TENDANG BOLA 1\n");
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 20);
        if (ball->is_caught == 0) {
            game_sub_status->value = 33;
        } else {
            if (pythagoras(final_pose->x, final_pose->y, pos_x_friend, pos_y_friend) < 500) {
                printf("MASUK PELAN %f \n", pythagoras(final_pose->x, final_pose->y, pos_x_friend, pos_y_friend));
                kickerControl(stm_data, THROTTLE_AIM, 7.5, 800);
            } else {
                kickerControl(stm_data, THROTTLE_AIM, 7.5, 800);
            }
            pos_x_kick_ball = final_pose->x;
            pos_y_kick_ball = final_pose->y;
        }
        break;
    /*
     * ===============================================================
     *                        ROBOT 2 UMPAN 1
     * ===============================================================
     */
    case 33:
        //? SEBELUM 470
        if (1 == bs_data->obs_bs_regional[2]) {
            if (bs_data->obs_bs_regional[1] == 7) {
                r2_recv_x = 740;
                r2_recv_y = 240; //* AMAN (+230)
            } else if (bs_data->obs_bs_regional[1] == 8) {
                r2_recv_x = 740;
                r2_recv_y = 320; //* AMAN (+150)
            }
            game_sub_status->value = 333;
            break;
        } else if (bs_data->obs_bs_regional[2] == 4) {
            if (bs_data->obs_bs_regional[1] == 7) {
                r2_recv_x = 740;
                r2_recv_y = 240; //* AMAN (+230)
            } else if (bs_data->obs_bs_regional[1] == 8) {
                r2_recv_x = 740;
                r2_recv_y = 360; // BAHAYA (+110)
            }
            game_sub_status->value = 333;
            break;
        } else if (2 == bs_data->obs_bs_regional[2]) {
            r2_recv_x = 730;
            r2_recv_y = 370; // BAHAYA (+100)
        } else {
            r2_recv_x = 730;
            r2_recv_y = 340; //* AMAN (+130)
        }

        game_sub_status->value = 4;
        break;
    case 333:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->ruckigToPoint3(r2_recv_x, final_pose->y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            game_sub_status->value = 4;
        }
        if (4 == bs_data->obs_bs_regional[2]) {
            game_sub_status->value = 4;
        }
        if (final_pose->x > 680) {
            game_sub_status->value = 4;
        }
        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, 180, 250, 190)) {
            game_sub_status->value = 44;
        }
        break;
    case 44:
        *robot_condition = 0;
        printf("TUNGGU TERIMA BOLA 2\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, ball->angle, 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        motion->motionToPoint(final_pose->x, final_pose->y, 180, 0, 90);
        if (shared_robot_data[3]->robot_condition == 11) {
            buzzerControl(stm_data, 10, 10);
            game_sub_status->value = 5;
        }
        break;
    case 5:
        printf("TERIMA BOLA 2\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint3(final_pose->x, final_pose->y, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
                game_sub_status->value = 55;
                pos_x_recv_ball = final_pose->x;
                pos_y_recv_ball = final_pose->y;
            }
        }
        break;
        /*
         * ===============================================================
         *                        ROBOT 1 UMPAN 2
         * ===============================================================
         */
    case 55:
        //* KEMUNGKINAN OBS SAMPING DI KIRI
        if (obs_samping == 5) {
            if (obs_tengah == 7) {
                r2_pass_x = 620;
                r2_pass_y = 220; //* AMAN (+120)
            } else if (obs_tengah == 8) {
                r2_pass_x = 570;
                r2_pass_y = 320; // BAHAYA (+20)

                r2_pass_y = pos_y_recv_ball - 70;
                if (r2_pass_y < 280)
                    r2_pass_y = 280;
            }
        } else if (obs_samping == 6) {
            if (obs_tengah == 8) {
                r2_pass_x = 580;
                r2_pass_y = 300; // BAHAYA (+40)

                r2_pass_y = pos_y_recv_ball - 70;
                if (r2_pass_y < 280)
                    r2_pass_y = 280;
            } else if (obs_tengah == 7) {
                r2_pass_x = 730;
                r2_pass_y = 80;
                // r2_pass_y = 230; //* AMAN (+110)

                // r2_pass_y = pos_y_recv_ball - 370;
                // if (r2_pass_y < 100)
            }
        } else if (obs_samping == 4) {
            if (obs_tengah == 8) {
                r2_pass_x = 620;
                r2_pass_y = 300; // BAHAYA (+60)

                r2_pass_y = pos_y_recv_ball - 70;
                if (r2_pass_y < 280)
                    r2_pass_y = 280;
            } else if (obs_tengah == 7) {
                r2_pass_x = 730;
                r2_pass_y = 200; // BAHAYA (+40)

                r2_pass_y = pos_y_recv_ball - 90;
                if (r2_pass_y < 120)
                    r2_pass_y = 120;
            }
        }

        //* KEMUNGKINAN OBS SAMPING DI KANAN
        if (obs_samping == 2) { // AMAN
            if (obs_tengah == 7) {
                r2_pass_x = 700;
                r2_pass_y = 170; //* AMAN (+200)
            } else if (obs_tengah == 8) {
                r2_pass_x = 700;
                r2_pass_y = 230; //* AMAN (+140)
            }
        } else if (obs_samping == 3) {
            r2_pass_x = 580;
            r2_pass_y = 300; // BAHAYA (+40)

            r2_pass_y = pos_y_recv_ball - 90;
            if (r2_pass_y < 250)
                r2_pass_y = 250;
        } else if (obs_samping == 1) {
            if (obs_tengah == 8) {
                r2_pass_x = 570;
                r2_pass_y = 280; // BAHAYA (+40)

                r2_pass_y = pos_y_recv_ball - 100; // BAHAYA (350)
                if (r2_pass_y < 240)
                    r2_pass_y = 240;
            } else if (obs_tengah == 7) {
                r2_pass_x = 730;
                r2_pass_y = 120; //* AMAN (+120)
            }
        }

        game_sub_status->value = 664;
        break;
    case 664:
        printf("PERSIAPAN TENDANG BOLA 2 SAFETY\n");

        if (final_pose->x > 650) {
            game_sub_status->value = 66;
            break;
        }

        if (motion->motionToPoint(r2_pass_x, final_pose->y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 300, 250)) {
            game_sub_status->value = 66;
        }
        break;
    case 66:
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(r3_recv_x, r3_recv_y, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
            game_sub_status->value = 666;
            printf("ATAS\n");
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_recv_ball, pos_y_recv_ball) > 100 && fabs(error_th) < 1 && obs_samping != 6 && obs_tengah != 7) {
            game_sub_status->value = 666;
            printf("BAWAH\n");
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 666:
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 20);
        printf("DIAM SEBENTAR\n");
        if ((obs_samping == 6 && obs_tengah == 7) || (obs_samping == 3 && obs_tengah == 8) || (obs_samping == 4 && obs_tengah == 8)) {
            game_sub_status->timeout(7, 0.5);
        } else {
            game_sub_status->timeout(7, 1);
        }
        break;
    case 7:
        *robot_condition = 11;
        printf("TENDANG BOLA 2\n");
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_friend, pos_y_friend), 20);
        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
        } else {
            if (final_pose->y < 150) {
                kickerControl(stm_data, THROTTLE_AIM, 9, 800);
            } else if (final_pose->x < 650) {
                kickerControl(stm_data, THROTTLE_AIM, 6.7, 800);
            } else {
                kickerControl(stm_data, THROTTLE_AIM, 7.5, 800);
            }
        }
        break;
    /*
     * ===============================================================
     *                             END
     * ===============================================================
     */
    case 8:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        // preparationGlobal();
        break;
    }
}

void Attacker::gameStyleC() const
{
    static float pos_x_recv_ball = 0;
    static float pos_y_recv_ball = 0;
    static float error_th = 0;

    static float r2_recv_x = 0;
    static float r2_recv_y = 0;
    static float r2_pass_x = 0;
    static float r2_pass_y = 0;

    static float r3_pass_x = 0;
    static float r3_pass_y = 0;
    static float r3_recv_x = 0;
    static float r3_recv_y = 0;

    static float pos_x_shoot = 0;
    static float pos_y_shoot = 0;

    float pos_x_friend = shared_robot_data[3]->pose.x;
    float pos_y_friend = shared_robot_data[3]->pose.y;

    r3_recv_x = pos_x_friend;
    r3_recv_y = pos_y_friend;
    float sudut_menghadap;

    float x_center;
    float y_center;

    int8_t obs_keeper = bs_data->obs_bs_regional[0];
    int8_t obs_center = bs_data->obs_bs_regional[1];
    int8_t obs_side = bs_data->obs_bs_regional[2];

    LogWithColor("cyan", "Attacker::gameStyleC() %d || %d %d || %d\n",
        game_sub_status->value, shared_robot_data[2]->robot_condition,
        shared_robot_data[3]->robot_condition, ball->status);

    if (pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y) < 100 && pos_x_recv_ball != 0 && pos_y_recv_ball != 0) {
        ROS_ERROR("STYLE C BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y));
    }

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    /*
     * ===============================================================
     *                       ROBOT 1 AMBIL
     * ===============================================================
     */
    case 0:
        // stm_data->global_kicker_position = 900;
        *robot_condition = 0;
        printf("PERSIAPAN AMBIL BOLA\n");
        if (motion->ruckigToPoint3(870, 110, 175, 250, 200)) {
            game_sub_status->value = 101;
        }

        if (pythagoras(final_pose->x, final_pose->y, 870, 110) < 50) {
            game_sub_status->value = 101;
        }
        break;
    case 101:
        printf("PERSIAPAN AMBIL BOLA\n");
        if (motion->ruckigToPoint(870, 30, 173, 100, 200)) {
            game_sub_status->value = 1;
            *robot_condition = 0;
        }

        // if (pythagoras(final_pose->x, final_pose->y, 850, 50) < 45) {
        //     game_sub_status->value = 1;
        //     *robot_condition = 0;
        // }

        // if (final_pose->y < 60)
        // {
        //     game_sub_status->value = 1;
        // }
        break;
    case 1:
        printf("CATCHING BALL\n");
        if (motion->catchingBall(60, 100)) {
            game_sub_status->value = 2;
        }
        break;
    case 2:
        // x_center = final_pose->x + 30 * cosf(final_pose->theta DEG2RAD);
        // y_center = final_pose->y + 30 * sinf(final_pose->theta DEG2RAD);

        // sudut_menghadap = atan2f(r3_recv_y - final_pose->y, r3_recv_x - final_pose->x) RAD2DEG;

        // r3_recv_x = 50;
        // r3_recv_y = 140; // 150

        // if (motion->pivotMotion(angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 30))
        // {
        //     game_sub_status->value = 3;
        // }
        if (motion->pivotMotion(/*angleToTarget(r3_recv_x, r3_recv_y, *final_pose)*/ 173, 100)) {
            game_sub_status->value = 3;
        }
        break;
    case 3:
        *robot_condition = 11;
        r3_recv_x = 50;
        r3_recv_y = 150;
        motion->pivotMotion(angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 2);
        printf("TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            game_sub_status->value = 33;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 9, 800);
        }
        break;
    /*
     * ===============================================================
     *                        ROBOT 2 UMPAN 1
     * ===============================================================
     */
    case 33:
        r2_recv_x = 770;
        r2_recv_y = 100;

        r3_pass_x = 100;
        r3_pass_y = 100;
        game_sub_status->value = 4;
        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 1\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            game_sub_status->value = 44;
        }
        break;
    case 44:
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 5;
        }
        break;
    case 5:
        printf("TERIMA BOLA 1\n");
        if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
            game_sub_status->value = 50;
            *robot_condition = 0;
            pos_x_recv_ball = final_pose->x;
            pos_y_recv_ball = final_pose->y;
        }
        break;
        /*
         * ===============================================================
         *                       ROBOT 1 UMPAN 1
         * ===============================================================
         */
    case 50:
        r2_pass_x = 650;
        r2_pass_y = 100;

        r3_recv_x = 100;
        r3_recv_y = 100;
        game_sub_status->value = 6;
        break;
    case 6:
        *robot_condition = 0;
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        // r3_recv_y += 25;
        error_th = angleToTarget(r3_recv_x, r3_recv_y, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint3(r2_pass_x, r2_pass_y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
            game_sub_status->value = 7;
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_recv_ball, pos_y_recv_ball) > 100 && fabs(error_th) < 1) {
            game_sub_status->value = 7;
        }

        if (pythagoras(final_pose->x, final_pose->y, r2_pass_x, r2_pass_y) < 50 && fabs(error_th) < 1) {
            game_sub_status->value = 7;
        }
        break;
    case 7:
        *robot_condition = 11;
        printf("TENDANG BOLA 2\n");
        // motion->manualMotion(0, 0, 0);
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(r3_recv_x, r3_recv_y), 30);
        if (ball->is_caught == 0) {
            game_sub_status->value = 77;
            game_sub_status->resetUptimeTimeout();
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 7.5, 800);
        }
        break;
    /*
     * ===============================================================
     *                          TAMBAH UMPAN
     * ===============================================================
     */
    case 77:
        r2_recv_x = 700;
        r2_recv_y = 100;

        r3_pass_x = 100;
        r3_pass_y = 100;

        pos_x_shoot = 470;
        pos_y_shoot = 0;

        if (3 == obs_keeper && 6 == obs_side) {
            game_sub_status->value = 8; //
            break;
        } else {
            game_sub_status->timeout(8, 0.2);
        }
        break;
    case 78:
        *robot_condition = 11;
        printf("PERSIAPAN TERIMA BOLA 3\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            game_sub_status->value = 79;
        }
        break;
    case 79:
        printf("TUNGGU TERIMA BOLA 3\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 80;
        }
        break;
    case 80:
        printf("TERIMA BOLA 3\n");
        if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
            game_sub_status->value = 810;
            *robot_condition = 0;
            pos_x_recv_ball = final_pose->x;
            pos_y_recv_ball = final_pose->y;
        }
        break;
    case 810:
        if (final_pose->y > 180) {
            game_sub_status->value = 81;
        }

        if (motion->ruckigToPoint3(final_pose->x, 200, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 90)) {
            game_sub_status->value = 81;
        }

        break;
    case 81:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint3(final_pose->x, final_pose->y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 155)) {
            game_sub_status->value = 82;
        }
        if (fabs(error_th) < 2 && pythagoras(final_pose->x, final_pose->y, final_pose->x, final_pose->y) < 60) {
            game_sub_status->value = 82;
        }
        break;
    case 82:
        printf("PERSIAPAN TENDANG GOAL\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint3(final_pose->x, final_pose->y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 155)) {
            game_sub_status->value = 83;
        }
        if (fabs(error_th) < 2.5) {
            game_sub_status->value = 83;
        }
        break;
    case 83:
        printf("TENDANG GOAL\n");
        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 20);
        }
        break;
    /*
     * ===============================================================
     *                             END
     * ===============================================================
     */
    case 8:
        if (bs_data->obs_bs_regional[2] != 3) {
            game_sub_status->value = 9;
        }
        if (motion->ruckigToPoint3(BASE_1.x + 10, final_pose->y, BASE_1.theta, 300, 250)) {
            game_sub_status->value = 9;
        }
        if (final_pose->x > 690) {
            game_sub_status->value = 9;
        }
        break;
    case 9:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        // preparationGlobal();
        break;
    }
}

void Attacker::gameStyleD() const
{
    static float pos_x_recv_ball = 0;
    static float pos_y_recv_ball = 0;
    static float error_th = 0;

    static float r2_recv_x = 0;
    static float r2_recv_y = 0;
    static float r2_pass_x = 0;
    static float r2_pass_y = 0;

    static float r3_pass_x = 0;
    static float r3_pass_y = 0;
    static float r3_recv_x = 0;
    static float r3_recv_y = 0;

    static float pos_x_shoot = 0;
    static float pos_y_shoot = 0;

    float pos_x_friend = shared_robot_data[3]->pose.x;
    float pos_y_friend = shared_robot_data[3]->pose.y;

    r3_recv_x = pos_x_friend;
    r3_recv_y = pos_y_friend;

    int8_t obs_keeper = bs_data->obs_bs_regional[0];
    int8_t obs_center = bs_data->obs_bs_regional[1];
    int8_t obs_side = bs_data->obs_bs_regional[2];

    LogWithColor("yellow", "Attacker::gameStyleD() %d || %d %d || %d %.2f %.2f [%.f %.f]\n",
        game_sub_status->value, shared_robot_data[2]->robot_condition,
        shared_robot_data[3]->robot_condition, ball->status, ball->pos.x, ball->pos.y,
        r3_recv_x, r3_recv_y);

    if (pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y) < 100 && pos_x_recv_ball != 0 && pos_y_recv_ball != 0) {
        ROS_ERROR("STYLE D BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_recv_ball, pos_y_recv_ball, r2_pass_x, r2_pass_y));
    }

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    /*
     * ===============================================================
     *                        ROBOT 2 AMBIL
     * ===============================================================
     */
    case 0:
        stm_data->global_kicker_position = 900;
        r2_recv_x = 770;
        r2_recv_y = 100;

        *robot_condition = 0;
        printf("PERSIAPAN TERIMA BOLA 1\n");
        if (motion->ruckigToPoint3(750, 120, angleToTarget(18, 18, *final_pose), 250, 150)) {
            game_sub_status->value = 11;
        }
        break;
    case 11:
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 1;
        }
        break;
    case 1:
        printf("TERIMA BOLA 1\n");
        if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
            game_sub_status->value = 10;
            pos_x_recv_ball = final_pose->x;
            pos_y_recv_ball = final_pose->y;
        }
        break;
    /*
     * ===============================================================
     *                        ROBOT 1 UMPAN 1
     * ===============================================================
     */
    case 10:
        r2_pass_x = 660;
        r2_pass_y = 100;
        game_sub_status->value = 2;
        break;
    // case 20:
    //     printf("PERSIAPAN TENDANG BOLA 1\n");
    //     if (ball->is_caught == 0) {
    //         motion->catchingBall(100, 90);
    //         break;
    //     }
    //     if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
    //         game_sub_status->value = 2;
    //     }
    //     if (pythagoras(final_pose->x, final_pose->y, r2_recv_x, r2_recv_y) > 50) {
    //         game_sub_status->value = 2;
    //     }
    //     if (pythagoras(final_pose->x, final_pose->y, pos_x_recv_ball, pos_y_recv_ball) > 50) {
    //         game_sub_status->value = 2;
    //     }
    //     break;
    case 2:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(r3_recv_x, r3_recv_y - 15, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        if (motion->ruckigToPoint3(r2_pass_x - 10, r2_pass_y, angleToTarget(r3_recv_x, r3_recv_y - 15, *final_pose), 200, 155)) {
            game_sub_status->value = 3;
            game_sub_status->resetUptimeTimeout();
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_recv_ball, pos_y_recv_ball) > 50 && fabs(error_th) < 1) {
            game_sub_status->value = 3;
            game_sub_status->resetUptimeTimeout();
        }
        break;
    case 3:
        *robot_condition = 11;
        printf("TENDANG BOLA 1\n");
        // motion->manualMotion(0, 0, 0);

        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(r3_recv_x, r3_recv_y - 15), 30);
        if (ball->is_caught == 0) {
            game_sub_status->value = 44;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 7.5, 800);
        }
        break;
    /*
     * ===============================================================
     *                       ROBOT 2 UMPAN 1
     * ===============================================================
     */
    case 44:
        r2_recv_x = 750;
        r2_recv_y = 100;

        r3_pass_x = 100;
        r3_pass_y = 120;
        game_sub_status->value = 4;
        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->motionToPoint(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 190)) {
            if (shared_robot_data[3]->robot_condition == 11) {
                game_sub_status->value = 55;
                *robot_condition = 0;
            }
        }
        break;
    case 55:
        *robot_condition = 0;
        printf("TUNGGU TERIMA BOLA 2\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 5;
        }
        break;
    case 5:
        *robot_condition = 0;
        printf("TERIMA BOLA 2\n");
        if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
            game_sub_status->value = 66;
        }
        break;
        /*
         * ===============================================================
         *                    ROBOT 1 GOAL || UMPAN 2
         * ===============================================================
         */
    case 66:
        if (bs_data->obs_bs_regional[0] == 1 && bs_data->obs_bs_regional[2] == 3) {
            pos_x_shoot = 365;
            pos_y_shoot = 0;
            r2_pass_x = 650;
            r2_pass_y = 350;
            game_sub_status->value = 67; //
        } /*else if (bs_data->obs_bs_regional[0] == 1 && bs_data->obs_bs_regional[2] == 2) {
            pos_x_shoot = 365;
            pos_y_shoot = 0;
            r2_pass_x = 650;
            r2_pass_y = final_pose->y;
            game_sub_status->value = 67;
        }*/
        else if (bs_data->obs_bs_regional[0] == 1) {
            pos_x_shoot = 360;
            pos_y_shoot = 0;
            r2_pass_x = 600;
            r2_pass_y = 200;
            game_sub_status->value = 67;
        } else {
            pos_x_shoot = 460;
            pos_y_shoot = 0;
            game_sub_status->value = 68;
        }
        break;
    case 67:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        if (motion->ruckigToPoint(r2_pass_x, r2_pass_y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 200)) {
            game_sub_status->value = 6;
        }
        break;
    case 68:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        // TAK GANTI jadi ruckig 2 DOF
        if (motion->ruckigToPoint(final_pose->x, 200, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 200)) {
            game_sub_status->value = 6;
        }
        // if (fabs(error_th) < 2.5 && pythagoras(final_pose->x, final_pose->y, final_pose->x, 200) < 50) {
        if (fabs(error_th) < 2 && pythagoras(final_pose->x, final_pose->y, final_pose->x, 200) < 30) {
            game_sub_status->value = 6;
        }
        break;
    case 6:
        printf("PERSIAPAN TENDANG GOAL\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        error_th = angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose) - final_pose->theta;
        if (error_th < -180) {
            error_th += 360;
        } else if (error_th > 180) {
            error_th -= 360;
        }
        // TAK GANTI jadi ruckig 2 DOF
        if (motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(pos_x_shoot, pos_y_shoot, *final_pose), 200, 155)) {
            game_sub_status->value = 6969;
        }
        if (fabs(error_th) < 1.5) {
            game_sub_status->value = 6969;
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 6969:
        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_shoot, pos_y_shoot), 15);
        game_sub_status->timeout(7, 0.4);
        break;
    case 7:
        printf("TENDANG GOAL\n");
        // motion->manualMotion(0, 0, 0);

        motion->motionAngleControl(0, 0, motion->RobotAngletoPoint(pos_x_shoot, pos_y_shoot), 15);
        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 20, 800);
        }
        break;
    /*
     * ===============================================================
     *                        TAMBAH UMPAN
     * ===============================================================
     */
    case 61:
        r2_recv_x = 700;
        r2_recv_y = 100;
        *robot_condition = 0;
        printf("PERSIAPAN TERIMA BOLA 3\n");
        if (motion->ruckigToPoint3(r2_recv_x, r2_recv_y, angleToTarget(r3_pass_x, r3_pass_y, *final_pose), 250, 150)) {
            game_sub_status->value = 62;
        }
        break;
    case 62:
        printf("TUNGGU TERIMA BOLA 3\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 63;
        }
        break;
    case 63:
        printf("TERIMA BOLA 3\n");
        if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
            game_sub_status->value = 69;
            pos_x_recv_ball = final_pose->x;
            pos_y_recv_ball = final_pose->y;
        }
        break;
    // ---
    case 69:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        if (motion->ruckigToPoint3(650, 110, angleToTarget(r3_recv_x, r3_recv_y, *final_pose), 200, 155)) {
            game_sub_status->value = 77;
            game_sub_status->resetUptimeTimeout();
        }
        break;
    case 77:
        printf("TENDANG UMPAN\n");
        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
            *robot_condition = 11;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 9.5, 800);
        }
        break;
    /*
     * ===============================================================
     *                             END
     * ===============================================================
     */
    case 8:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        // preparationGlobal();
        break;
    }
}

void Attacker::gameStyleE() const
{
    LogWithColor("blue", "Attacker::gameStyleE() %d || %d %d || %d %.f %.f [SUDUT : %.2f -> %.2f]\n",
        game_sub_status->value, shared_robot_data[2]->robot_condition,
        shared_robot_data[3]->robot_condition, ball->status, ball->pos.x, ball->pos.y,
        (bs_data->obs_bs_regional[0] == 1) ? atan2f(0 - 460, 360 - 760) RAD2DEG : (bs_data->obs_bs_regional[0] == 2) ? atan2f(0 - 460, 470 - 760) RAD2DEG
                                                                                                                     : atan2f(0 - 460, 400 - 760) RAD2DEG,
        motion->RobotAngletoPoint(360, 0));

    static float pos_x_friend = shared_robot_data[3]->pose.x;
    static float pos_y_friend = shared_robot_data[3]->pose.y;

    static float tendang_obs_1 = 360;
    static float tendang_obs_2 = 460;
    static float tendang_obs_3 = 420;

    static float error_th = 0;

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }
#define PENENDANG
#ifndef PENENDANG
    switch (game_sub_status->value) {
    case 0:
        stm_data->global_kicker_position = 900;
        *robot_condition = 0;
        // printf("AMBIL BOLA\n");
        // if (motion->ruckigToPoint3(480, 650, -170, 250, 200)) {
        //     game_sub_status->value = 101;
        // }

        // if (pythagoras(final_pose->x, final_pose->y, 480, 650) < 150) {
        //     game_sub_status->value = 101;
        // }

        // if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 150) {
        //     game_sub_status->value = 101;
        // }
        game_sub_status->value = 123;
        break;
    case 123:
        if (motion->motionToPoint(480, 600, 180, 300, 90)) {
            game_sub_status->value = 124;
        }
        if (motion->RobotDist2Point(480, 600) < 120) {
            game_sub_status->value = 124;
        }
        break;
    case 124:
        if (motion->motionToPoint(FIELD_X_1_2 + 20, FIELD_Y_1_2 + 50, -170, 150, 90)) {
            game_sub_status->value = 125;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1_2) < 100) {
            game_sub_status->value = 125;
        }
        break;
    case 125:
        if (motion->motionAroundBall(-170, ball->angle, 65, 120, 90)) {
            game_sub_status->value = 1;
        }
        {
            float error_robot_hadap = -170 - final_pose->theta;
            while (error_robot_hadap > 180)
                error_robot_hadap -= 360;
            while (error_robot_hadap < -180)
                error_robot_hadap += 360;

            if (fabs(error_robot_hadap) < 8) {
                game_sub_status->value = 1;
            }
        }
        break;
    case 101:
        *robot_condition = 0;
        printf("AMBIL BOLA\n");
        if (motion->ruckigToPoint3(ball->pos.x + 100, ball->pos.y, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 100, 200)) {
            game_sub_status->value = 1;
        }
        break;
    case 1:
        // printf("TEST %.2f %.2f\n", ball->pos.y, final_pose->y);
        // if (ball->pos.y > final_pose->y) {
        //     motion->motionToPoint(final_pose->x, ball->pos.y + 40, ball->angle, 100, 90);
        //     break;
        // }
        printf("CATCHING BALL\n");
        if (motion->catchingBall(80, 19)) {
            game_sub_status->value = 2; // 2  2
        }
        break;
    case 2:
        if (motion->pivotMotion(angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 180)) {
            game_sub_status->value = 3;
        }
        error_th = angleToTarget(pos_x_friend, pos_y_friend, *final_pose) - final_pose->theta;

        if (error_th > 180)
            error_th -= 360;
        if (error_th < -180)
            error_th += 360;

        if (fabs(error_th) < 3) {
            game_sub_status->value = 3;
        }
        break;
    case 3:
        printf("TENDANG BOLA 1\n");
        // motion->manualMotion(0, 0, 0);
        motion->motionAngleControl(0, 0, angleToTarget(pos_x_friend, pos_y_friend, *final_pose), 20);

        if (ball->is_caught == 0) {
            game_sub_status->value = 100;
            *robot_condition = 11;
        } else {
            kickerControl(stm_data, NO_AIM, 8, 900);
        }
        break;
    case 100:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
#else
    switch (game_sub_status->value) {
        /*
         * ===============================================================
         *                             TERIMA
         * ===============================================================
         */
    case 0:
        stm_data->global_kicker_position = 900;
        *robot_condition = 0;
        printf("PERSIAPAN TERIMA BOLA 1\n");
        if (motion->ruckigToPoint3(750, 460, angleToTarget(400, 600, *final_pose), 200, 155)) {
            game_sub_status->value = 2;
        }
        break;
    case 2:
        stm_data->global_kicker_position = 200;
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[3]->robot_condition == 11) {
            game_sub_status->value = 4;
        }
        break;
    case 4:
        stm_data->global_kicker_position = 200;
        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[3]->pose.x, shared_robot_data[3]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(pos_x_friend, pos_y_friend, 100, 90)) {
                game_sub_status->value = 5;
            }
        }
        break;
        /*
         * ===============================================================
         *                            GERAK
         * ===============================================================
         */
    case 5:
        stm_data->global_kicker_position = 200;

        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (bs_data->obs_bs_regional[0] == 1) {
            motion->motionToPoint(750, 460, angleToTarget(tendang_obs_1, 0, *final_pose), 200, 155);
        } else if (bs_data->obs_bs_regional[0] == 2) {
            motion->motionToPoint(750, 460, angleToTarget(tendang_obs_2, 0, *final_pose), 200, 155);
        } else {
            motion->motionToPoint(750, 460, angleToTarget(tendang_obs_3, 0, *final_pose), 200, 155);
        }
        {
            float sudut_kiri;
            float sudut_kanan;

            float error_th;
            if (bs_data->obs_bs_regional[0] == 1) {
                // error_th = motion->RobotAngletoPoint(tendang_obs_1, 0) - final_pose->theta;
                sudut_kiri = motion->RobotAngletoPoint(420, 0);
                sudut_kanan = motion->RobotAngletoPoint(320, 0);
            } else if (bs_data->obs_bs_regional[0] == 2) {
                // error_th = motion->RobotAngletoPoint(tendang_obs_2, 0) - final_pose->theta;
                sudut_kiri = motion->RobotAngletoPoint(500, 0);
                sudut_kanan = motion->RobotAngletoPoint(450, 0);
            } else {
                // error_th = motion->RobotAngletoPoint(tendang_obs_3, 0) - final_pose->theta;
                sudut_kiri = motion->RobotAngletoPoint(500, 0);
                sudut_kanan = motion->RobotAngletoPoint(400, 0);
            }

            // while (error_th > 180) {
            //     error_th -= 360;
            // }
            // while (error_th < -180) {
            //     error_th += 360;
            // }

            if (pythagoras(final_pose->x, final_pose->y, 770, 450) < 50 && (final_pose->theta > sudut_kanan && final_pose->theta < sudut_kiri)) {
                game_sub_status->value = 88;
            }
        }

        break;
        /*
         * ===============================================================
         *                            TENDANG
         * ===============================================================
         */
    case 88:
        printf("TENDANG BOLA 1\n");
        if (bs_data->obs_bs_regional[0] == 1) {
            motion->motionAngleControl(0, 0, angleToTarget(tendang_obs_1, 0, *final_pose), 15);
        } else if (bs_data->obs_bs_regional[0] == 2) {
            motion->motionAngleControl(0, 0, angleToTarget(tendang_obs_2, 0, *final_pose), 15);
        } else {
            motion->motionAngleControl(0, 0, angleToTarget(tendang_obs_3, 0, *final_pose), 15);
        }

        if (ball->is_caught == 0) {
            game_sub_status->value = 100;
        } else {
            if (bs_data->obs_bs_regional[0] == 1) {
                printf("KENCANG\n");
                kickerControl(stm_data, KICKER_POSITION_AIM | THROTTLE_AIM, 20, 200);
            } else {
                printf("PELAN\n");
                kickerControl(stm_data, KICKER_POSITION_AIM | THROTTLE_AIM, 17, 200);
            }
        }

        break;
    case 99:
        game_sub_status->timeout(100, 0);
        motion->manualMotion(0, 0, 0);
        break;
    case 100:
        if (motion->ruckigToPoint3(BASE_1.x, BASE_1.y, BASE_1.theta, 300, 250)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
#endif
}