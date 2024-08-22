

#include "master/roles/defender.hpp"

void Defender::gameGeneralHomeSingle() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::preparationAwaySingle() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::gameGeneralAwaySingle() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::preparationHomeSingle() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::gameGeneralHomeDouble() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::preparationHomeDouble() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::gameGeneralAwayDouble() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

void Defender::preparationAwayDouble() const
{
    printf("Defender::gameGeneral() %d\n", game_status->value);
}

//=--------Prep Home--------=//
void Defender::PrepKickoffHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.05);
    printf("Defender::PrepKickoffHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 110;
            break;
        case 'B':
            game_sub_status_prep->value = 100;
            break;
        // case 'F':
        //     game_sub_status_prep->value = 600;
        //     break;
        default:
            game_sub_status_prep->value = 100;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 100:
        if (motion->ruckigToPoint(FIELD_X_2_3, FIELD_Y_1_3, 90, 100, 90, NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 110;
        }
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100) {
            game_sub_status_prep->value = 110;
        }
        break;

        //===============================================================================================

    case 110:
        if (ball->status == 0) {
            counter++;
        }
        motion->ruckigToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70);
        if (ball->distance < 200) {
            game_sub_status_prep->value = 120;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;

    case 120:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(165, motion->RobotAngletoBall(), 70, 100, 70, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepKickoffHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("Defender::PrepKickoffHomeDouble(): %d\n", game_sub_status_prep->value);
    *robot_condition = 0;

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

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
        if (motion->ruckigToPoint(FIELD_X_1_3, FIELD_Y_1_3, 90, 200, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 120;

        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100 && motion->RobotDist2Point(FIELD_X_1_2, FIELD_Y_1_2) < 200)
            game_sub_status_prep->value = 120;
        break;

    case 110:
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100) {
            printf("Masuk liat bola\n");
            if (motion->motionAroundPointAngle(ball->pos.x, ball->pos.y, 250, 250, 10, 150, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
                game_sub_status_prep->value = 120;
        } else {
            printf("Masuk ga liat bola\n");
            if (motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, 250, 250, 10, 150, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
                game_sub_status_prep->value = 120;
        }
        break;
    case 120:
        if (ball->status > 0) {
            printf("Masuk liat bola\n");
            if (motion->motionAroundPointAngle(ball->pos.x, ball->pos.y, 10, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 60, 100, 90, NORMAL_OBSTACLE)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            printf("Masuk ga liat bola\n");

            if (motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, 10, angleToTarget(FIELD_X_1_2, FIELD_Y_1_2, *final_pose), 60, 100, 90, NORMAL_OBSTACLE)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        break;
    default:
        game_sub_status_prep->value = 100;
        break;
    }
}

void Defender::PrepKickoffHomeTriple() const { }
void Defender::PrepKickoffHomeQuadruple() const { }

//==========================================//
//=--------Preparation Freekick Home-------=//
//==========================================//
void Defender::PrepFreekickHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.04);
    printf("Defender::PrepFreekickHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        case 'B':
            game_sub_status_prep->value = 100;
            break;
            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 100:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 150, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
            game_sub_status_prep->value = 101;
        }
        break;

        //===============================================================================================

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 102;
        }
        if (ball->distance <= 300) {
            game_sub_status_prep->value = 102;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal(), motion->RobotAngletoBall(), 60, 120, 60)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepFreekickHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    *robot_condition = 0;

    printf("Defender::PrepFreekickHomeDouble() : %d || %.2f || %d \n", game_sub_status_prep->value, pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y), bs_data->n_robot_att);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;

        // if (bs_data->style == 'F')
        // {
        //     game_sub_status_prep->value = 600;
        // }

        printf("prep freekick home double\n");
        break;
    case 1:
        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 100 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 100 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100)) {
            game_sub_status_prep->value = 2;
        }

        // GERAK KE TENGAH LAPANGAN
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_4, 90, 200, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 2;
        }

        break;
    case 2: // JIKA ADA BOLA
        // MENDEKAT KE BOLA
        if (motion->ruckigToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 200, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 3;
        }
        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 300) {
            game_sub_status_prep->value = 3;
        }

        if (ball->status == 0)
            game_sub_status_prep->value = 1;

        break;
    case 3:
        printf("%.2f %.2f || %.2f %.2f \n", shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, ball->pos.x, ball->pos.y);

        // PUTARI BOLA
        if (motion->motionAroundBall(
                atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
                motion->RobotAngletoBall(),
                65, 100, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }

        // motion->motionAroundBall(
        //     atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
        //     motion->RobotAngletoBall(),
        //     90, 100, 90, NORMAL_OBSTACLE_FRIEND);
        break;

    default:
        break;
    }
}

void Defender::PrepFreekickHomeTriple() const { }
void Defender::PrepFreekickHomeQuadruple() const { }

//==========================================//
//=--------Preparation Goalkick Home-------=//
//==========================================//

void Defender::PrepGoalkickHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.04);
    printf("Defender::PrepGoalkickHomeSingle() %d %.2f\n", game_sub_status_prep->value, ball->distance);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        case 'B':
            game_sub_status_prep->value = 100;
            break;

            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        counter = 0;
        break;

        //===============================================================================================

    case 100:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE))
            game_sub_status_prep->value = 101;

        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1_4) {
            game_sub_status_prep->value = 101;
        }
        break;

        //===============================================================================================

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 102;
            counter = 0;
        }
        if (ball->distance <= 170) {
            game_sub_status_prep->value = 102;
            counter = 0;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (ball->pos.x < FIELD_X_1_2) {
            if (motion->motionAroundBall(-5, motion->RobotAngletoBall(), 70, 120, 90)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(-175, motion->RobotAngletoBall(), 70, 120, 90)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}
void Defender::PrepGoalkickHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    *robot_condition = 0;

    printf("Defender::PrepGoalkickHomeDouble : %d || %.2f || %d \n", game_sub_status_prep->value, pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y), bs_data->n_robot_att);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        printf("Defender::PrepGoalkickHomeDouble \n");
        game_sub_status_prep->value = 1;

        // if (bs_data->style == 'F')
        // {
        //     game_sub_status_prep->value = 600;
        // }
        break;
    case 1:
        if (ball->status > 0 && ball->pos.x < FIELD_X_1 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.y < FIELD_Y_1_3) {
            game_sub_status_prep->value = 2;
        }

        // GERAK KE TENGAH LAPANGAN
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_3, 90, 170, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 2;
        }

        break;
    case 2: // JIKA ADA BOLA
        // MENDEKAT KE BOLA
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 3;
        }
        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 200) {
            game_sub_status_prep->value = 3;
        }

        if (ball->status == 0)
            game_sub_status_prep->value = 1;

        break;
    case 3:
        printf("%.2f %.2f || %.2f %.2f \n", shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, ball->pos.x, ball->pos.y);

        // PUTARI BOLA
        if (
            motion->motionAroundBall(
                atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
                motion->RobotAngletoBall(),
                65, 120, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;

    default:
        break;
    }
}

void Defender::PrepGoalkickHomeTriple() const { }
void Defender::PrepGoalkickHomeQuadruple() const { }

//========================================//
//=--------Preparation Corner Home-------=//
//========================================//

void Defender::PrepCornerHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.05);
    printf("Devender::PrepCornerHomeSingle() %d %d %.2f %.2f\n", game_sub_status_prep->value, ball->status, ball->pos.x, ball->pos.y);

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status == 0) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 100;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 90, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE))
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
            if (motion->motionAroundBall(45, motion->RobotAngletoBall(), 70, 120, 90)) {
                game_sub_status_prep->value = 102;
            }
            if (deltaAngle(final_pose->theta, 45) < 15) {
                game_sub_status_prep->value = 102;
            }
        } else {
            if (motion->motionAroundBall(135, motion->RobotAngletoBall(), 70, 120, 90)) {
                game_sub_status_prep->value = 102;
            }
            if (deltaAngle(final_pose->theta, 135) < 15) {
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
            if (motion->motionAroundBall(-45, motion->RobotAngletoBall(), 70, 60, 90)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(-135, motion->RobotAngletoBall(), 70, 60, 90)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepCornerHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    *robot_condition = 0;

    printf("prep corner home double || %d %.2f %d %.2f %.2f \n", game_sub_status_prep->value, pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y), ball->status, ball->pos.x, ball->pos.y);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;

        // if (bs_data->style == 'F')
        // {
        //     game_sub_status_prep->value = 600;
        // }
        printf("prep CORNER home double\n");
        break;
    case 1:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_2 + 300, 90, 200, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 21;
        }

        break;
    case 2:
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 3;
        }

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 120) {
            game_sub_status_prep->value = 3;
        }

        if (ball->status == 0)
            game_sub_status_prep->value = 1;

        break;
    case 21:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint(FIELD_X_1_4, FIELD_Y_4_5, angleToTarget(0, 1200, *final_pose), 200, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 22;
        }

        break;
    case 22:
        if (ball->status > 0 && ball->pos.y > FIELD_Y_3_4) {
            game_sub_status_prep->value = 2;
        }

        if (motion->ruckigToPoint(FIELD_X_1_2 + 200, FIELD_Y_4_5, angleToTarget(800, 1200, *final_pose), 200, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 21;
        }
        break;
    case 3:
        printf("%.2f %.2f || %.2f %.2f \n", shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, ball->pos.x, ball->pos.y);

        if (motion->motionAroundBall(
                atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
                motion->RobotAngletoBall(),
                65, 90, 90, NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }

        // motion->motionAroundBall(
        //     atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
        //     motion->RobotAngletoBall(),
        //     90, 100, 90, NORMAL_OBSTACLE_FRIEND);

        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;

    default:
        break;
    }
}
void Defender::PrepCornerHomeTriple() const { }
void Defender::PrepCornerHomeQuadruple() const { }

//=========================================//
//=--------Preparation Throwin Home-------=//
//=========================================//

void Defender::PrepThrowinHomeSingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.1);

    printf("Defender::PrepThrowinHomeSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status == 0) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 100;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (final_pose->y < FIELD_Y_1_2) {
            if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 180, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
                game_sub_status_prep->value = 100;
            }
        } else {
            if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 180, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
                game_sub_status_prep->value = 100;
            }
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE))
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
        if (motion->motionAroundBall(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_3_4), motion->RobotAngletoBall(), 70, 120, 90)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepThrowinHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("throw in home double assist %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 100;

        // if (bs_data->style == 'F')
        // {
        //     game_sub_status_prep->value = 600;
        // }
        break;

    case 100:
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_2, 90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 101;
        }

        if (ball->status) {
            game_sub_status_prep->value = 101;
        }
        break;

    case 101:
        if (motion->motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 102;
        }

        if (ball->distance <= 130) {
            game_sub_status_prep->value = 102;
        }

        if (ball->status == 0)
            game_sub_status_prep->value = 100;
        break;

    case 102:
        if (motion->motionAroundBall(atan2f(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG, atan2f(FIELD_Y_1_2 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG, 70, 90, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 103;
        }
        break;
    case 103:
        printf("%.2f %.2f || %.2f %.2f \n", shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, ball->pos.x, ball->pos.y);

        if (motion->motionAroundBall(
                atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
                motion->RobotAngletoBall(),
                68, 90, 90, NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }

        break;

        // motion->motionAroundBall(
        //     atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
        //     motion->RobotAngletoBall(),
        //     90, 100, 90, NORMAL_OBSTACLE_FRIEND);
        // break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepThrowinHomeTriple() const { }
void Defender::PrepThrowinHomeQuadruple() const { }

//=========================================//
//=--------Preparation Penalty Home-------=//
//=========================================//

void Defender::PrepPenaltyHomeSingle() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("PrepPenaltyHomeSingle : %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
    case 100:

        motion->manualMotion(0, 0, 0);

        break;
    default:
        break;
    }
}

void Defender::PrepPenaltyHomeDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("PrepPenaltyHomeDouble : %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;
        default:
            game_sub_status_prep->value = 100;
            break;
        }
    case 100:
        if (motion->motionToPoint(FIELD_X_1_2, (FIELD_Y_1_5), 90, 150, 90, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }

        break;
    default:
        break;
    }
}

void Defender::PrepPenaltyHomeTriple() const { }
void Defender::PrepPenaltyHomeQuadruple() const { }

//=--------Prep Away--------=//

//=========================================//
//=--------Preparation Kickoff Away-------=//
//=========================================//

void Defender::PrepKickoffAwaySingle() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("Defender::PrepKickoffAwaySingle() : %d \n", game_sub_status->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    // if (bs_data->style == 'F')
    // {
    //     game_sub_status_prep->value = 600;
    // }

    switch (game_sub_status_prep->value) {

        //==================================================================================================================

    case 0:
        motion->ruckigToPoint3(FIELD_X_1_2 + 120, FIELD_Y_1_2 - 200, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 125, 90, NORMAL_OBSTACLE_BALL);
        // if (motion->ruckigToPoint3(FIELD_X_1_2 + 120, FIELD_Y_1_2 - 150, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 125, 90)) {
        //     game_sub_status = 10;
        // }
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_3, motion->RobotAngletoBall(), 190, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 1;
        }
        if (ball->distance < 250 && ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) < 100) {
            game_sub_status_prep->value = 1;
        }
        break;

        //==================================================================================================================

    case 1:
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 120, 90, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}
void Defender::PrepKickoffAwayDouble() const
{
    game_sub_status->reentry(0, 0.1);

    printf("PREP k: %d\n", game_sub_status->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status->value) {
    case 0:

        if (motion->ruckigToPoint(FIELD_X_1_3, FIELD_Y_1_4 - 50, 90, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        // PotentialMotion(FIELD_X_1_3, FIELD_Y_1_4 - 50, 90, 90, 100, NORMAL_OBSTACLE);
        break;

    default:
        break;
    }
}

void Defender::PrepKickoffAwayTriple() const { }
void Defender::PrepKickoffAwayQuadruple() const { }

//==========================================//
//=--------Preparation Freekick Away-------=//
//==========================================//

void Defender::PrepFreekickAwaySingle() const
{
    static int counter = 0;

    game_sub_status_prep->reentry(0, 0.05);
    printf("Defender::PrepFreekickAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 110;
            break;
        case 'B':
            game_sub_status_prep->value = 100;
            break;
        // case 'F':
        //     game_sub_status_prep->value = 600;
        //     break;
        default:
            game_sub_status_prep->value = 100;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 100:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 150, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_1_4 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_3_4) {
            game_sub_status_prep->value = 101;
        }
        break;

        //==================================================================================================================

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 102;
        }
        if (ball->distance <= 350) {
            game_sub_status_prep->value = 102;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 320, 120, 60, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepFreekickAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("FREEKICK AWAY %d \n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;
    case 100:

        // if (PotentialMotion(FIELD_X_1_2, FIELD_Y_1_4, 90, 150, 90))
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
                game_sub_status_prep->value = 101;
            } else {
                game_sub_status_prep->value = 102;
            }
        }

        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
            game_sub_status_prep->value = 101;
        }
        break;
    case 101:
        if (motion->motionAroundBall(atan2(ball->pos.y - (-100), ball->pos.x - 400) RAD2DEG, atan2(ball->pos.y - (-100), ball->pos.x - 400) RAD2DEG, 400, 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }

        if (ball->status == 0)
            game_sub_status_prep->value = 102;

        break;
    case 102:

        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_0 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
            game_sub_status_prep->value = 101;
        }

        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
        break;
    default:
        break;
    }
}

void Defender::PrepFreekickAwayTriple() const { }
void Defender::PrepFreekickAwayQuadruple() const { }

//==========================================//
//=--------Preparation Goalkick Away-------=//
//==========================================//
void Defender::PrepGoalkickAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.1);
    printf("DEFENDER:GOALKICK AWAY SINGLE %d \n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 110;
            break;
        case 'B':
            game_sub_status_prep->value = 100;
            break;
        // case 'F':
        //     game_sub_status_prep->value = 600;
        //     break;
        default:
            game_sub_status_prep->value = 100;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 100:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;
        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_1_2 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
            game_sub_status_prep->value = 101;
        }
        break;

        //==================================================================================================================

    case 101:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 102;
        }
        if (ball->distance <= 300) {
            game_sub_status_prep->value = 102;
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;

    case 102:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 400, 120, 60, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 100;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepGoalkickAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("Defender::Prep Goalkick Away Double %d %d\n", game_sub_status_prep->value, ball->status);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;
    case 100:

        // PotentialMotion(FIELD_X_1_2, FIELD_Y_1_4, 90, 150, 90);
        if (ball->status > 0 && ball->pos.x > FIELD_X_0 && ball->pos.y > FIELD_Y_1_2 && ball->pos.x < FIELD_X_1 && ball->pos.y < FIELD_Y_1) {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 220, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, 90, 220, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }
        break;
    case 101:
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_2_3, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    default:
        break;
    }
}

void Defender::PrepGoalkickAwayTriple() const { }
void Defender::PrepGoalkickAwayQuadruple() const { }

//========================================//
//=--------Preparation Corner Away-------=//
//========================================//

void Defender::PrepCornerAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.055);
    printf("Defender::PrepCornerAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;

            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status == 0) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 100;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_4, -90, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
            game_sub_status_prep->value = 100;
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->motionAroundBall(atan2(ball->pos.y - FIELD_Y_1_2, ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 320, 120, 70, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepCornerAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);
    printf("PrepCornerAwayDouble  || %d\n", game_sub_status_prep->value);

    if (ball->status == 0) {
        game_sub_status_prep->value = 0;
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;
    case 100:
        if (ball->status > 0) {
            game_sub_status_prep->value = 105;
        }
        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_2, 90, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
        break;
    case 105:
        if (final_pose->y < 190) {
            if (ball->pos.x < FIELD_X_1_2) {
                motion->motionToPoint(FIELD_X_1_4, FIELD_Y_1_5, motion->RobotAngletoBall(), 70, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
            }

            if (ball->pos.x > FIELD_X_1_2) {
                motion->motionToPoint(FIELD_X_3_4, FIELD_Y_1_5, motion->RobotAngletoBall(), 70, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
            }
        } else {
            if (ball->pos.x < FIELD_X_1_2) {
                motion->motionToPoint(FIELD_X_1_4, FIELD_Y_1_5, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
            }

            if (ball->pos.x > FIELD_X_1_2) {
                motion->motionToPoint(FIELD_X_3_4, FIELD_Y_1_5, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
            }
        }
        break;

    default:
        break;
    }
}

void Defender::PrepCornerAwayTriple() const { }
void Defender::PrepCornerAwayQuadruple() const { }

//=========================================//
//=--------Preparation Throwin Away-------=//
//=========================================//

void Defender::PrepThrowinAwaySingle() const
{
    static int counter = 0;
    game_sub_status_prep->reentry(0, 0.1);
    printf("Defender::PrepThrowinAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 10;
            break;

        case 'B':
            game_sub_status_prep->value = 10;
            break;
            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 10;
            break;
        }
        counter = 0;
        break;

        //==================================================================================================================

    case 10:
        if (ball->status == 0) {
            game_sub_status_prep->value = 11;
        } else {
            game_sub_status_prep->value = 100;
        }
        break;

    case 11:
        if (ball->status > 0 || bs_data->n_nearest_ball > 0) {
            game_sub_status_prep->value = 100;
        }

        if (final_pose->y < FIELD_Y_1_2) {
            if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 180, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
                game_sub_status_prep->value = 100;
            }
        } else {
            if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_3_4, 180, 120, 70, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE)) {
                game_sub_status_prep->value = 100;
            }
        }
        break;

        //==================================================================================================================

    case 100:
        if (ball->status == 0) {
            counter++;
        }
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 150, 70, NORMAL_OBSTACLE))
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
        if (motion->motionAroundBall(motion->BallAngletoOurGoal() - 180, motion->RobotAngletoBall(), 200, 120, 90, NORMAL_OBSTACLE)) {
            motion->manualMotion(0, 0, 0);
        }
        if (counter > 150) {
            game_sub_status_prep->value = 10;
            counter = 0;
        }
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepThrowinAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("throw in away double assist %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        game_sub_status_prep->value = 1;
        break;

    case 1:
        if (ball->status > 0) {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
                motion->manualMotion(0, 0, 0);
            }
        }

        break;
    }
}
void Defender::PrepThrowinAwayTriple() const { }
void Defender::PrepThrowinAwayQuadruple() const { }

//=========================================//
//=--------Preparation Penalty Away-------=//
//=========================================//

void Defender::PrepPenaltyAwaySingle() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Defender::PrepPenaltyAwaySingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        motion->motionToPoint(FIELD_X_1_4, FIELD_Y_1_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);

        break;

    default:
        break;
    }
}
void Defender::PrepPenaltyAwayDouble() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Defender::PrepPenaltyAwayDouble() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        motion->motionToPoint(FIELD_X_1_4, FIELD_Y_1_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);

        break;

    default:
        break;
    }
}
void Defender::PrepPenaltyAwayTriple() const { }
void Defender::PrepPenaltyAwayQuadruple() const { }

//=========================================//
//=---------Preparation Dropball----------=//
//=========================================//
void Defender::PrepDropBallSingle() const
{
    game_sub_status_prep->reentry(0, 0.055);
    printf("Defender::PrepDropBallSingle() %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    switch (game_sub_status_prep->value) {
    case 0:
        switch (bs_data->style) {
        case 'A':
            game_sub_status_prep->value = 100;
            break;

        case 'B':
            game_sub_status_prep->value = 100;
            break;
            // case 'F':
            //     game_sub_status_prep->value = 600;
            //     break;

        default:
            game_sub_status_prep->value = 100;
            break;
        }
        break;

    case 100:
        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        }

        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        break;

    case 101:
        if (motion->ruckigToPoint3(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 150) {
            game_sub_status_prep->value = 105;
        }

        break;
    case 103:
        if (motion->ruckigToPoint3(FIELD_X_1_2, FIELD_Y_1_2 + 100, -90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        }
        break;
    case 102:
        if (motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, 250, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            game_sub_status_prep->value = 105;
        }

        break;
    case 105:
        motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, 100, 120, 70, NORMAL_OBSTACLE);
        break;
    case 600:
        printf("BS-STYLE F\n");
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_4, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::PrepDropBallDouble() const
{
    game_sub_status_prep->reentry(0, 0.1);

    printf("Prep Drop Ball Double ASSIST stat: %d\n", game_sub_status_prep->value);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

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
        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        }
        {
            game_sub_status_prep->value = 101;
        }
        {
            game_sub_status_prep->value = 105;
        }
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL))
            game_sub_status_prep->value = 105;

        break;

    case 101:
        if (motion->ruckigToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 105;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 350) {
            game_sub_status_prep->value = 105;
        }

        if (ball->status == 0) {
            game_sub_status_prep->value = 100;
        }

        break;
    case 104:

        break;
    case 103:
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_2 + 100, -90, 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND))
            game_sub_status_prep->value = 101;

        if (ball->status > 0 && (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_4, FIELD_Y_1_2) < 150 || pythagoras(ball->pos.x, ball->pos.y, FIELD_X_3_4, FIELD_Y_1_2) < 150)) {
            game_sub_status_prep->value = 101;
        } else {
            game_sub_status_prep->value = 100;
        }
        break;
    case 102:
        if (motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, 250, 100, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL)) {
            game_sub_status_prep->value = 105;
        }

        if (ball->status == 0) {
            game_sub_status_prep->value = 100;
        }

        break;
    case 105:

        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_5, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 150, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND)) {
            motion->manualMotion(0, 0, 0);
        }

        if (ball->status == 0) {
            game_sub_status_prep->value = 100;
        }

        // MotionAroundBall(350, atan2(ball->pos.y - 0, ball->pos.x - 400)  RAD2DEG, 120, 70, NORMAL_OBSTACLE);
        break;
    }
}

void Defender::PrepDropBallTriple() const { }
void Defender::PrepDropBallQuadruple() const { }

void Defender::PrepparationGlobalDynamic() const
{
    printf("Defender :: PrepparationGlobalDynamic() \n");

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    game_sub_status_prep->reentry(0, 0.5);

    switch (game_sub_status_prep->value) {
    case 0:
        if (*titik_final_positioning[0] > FLT_EPSILON && *titik_final_positioning[1] > FLT_EPSILON) {
            if (ball->status > 0)
                motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoBall(), 100, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
            else
                motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], 90, 100, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);
        }
        break;
    }
}

//=--------Main--------=//
void Defender::GameMainSingle() const
{
    static int prev_n_has_ball;
    static uint64_t pass_timer[2];
    static uint64_t time_update = 0;
    static float ball_buffer[2];
    static float pos_robot_buffer[2];
    static obstacle_checking_t obs;
    static const int ring_rebut_bola_min = 0;
    static const int ring_rebut_bola_max = 300;

    static float arah_target = 0;

    // OBSTACLE AVOIDANCE MENGHADAP BOLA
    obs = motion->obstacleChecking(motion->RobotAngletoBall(), 30, ball->distance + 50, nearest_pixel);

    game_sub_status->reentry(0, 0.05);
    printf("Defender::GameMainSingle() %d || %d\n", game_sub_status->value, *has_prep);

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
        if (*has_prep == 0) { // Home

            game_sub_status->value = 1;
        } else if (*has_prep == 2) // Away
            game_sub_status->value = 2;
        else if (*has_prep == 3) // Special
        {
            game_sub_status->value = 3;
        }

        game_sub_status->resetUptimeTimeout();

        break;
    case 1:
        /**
         *? HOME
         * CATCHING BALL
         * NEXT STATE -> 10
         */

        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
            motion->catchingBall(10, 10);
        } else {
            motion->catchingBall(120, 90);
        }

        if (ball->is_caught == 2) {
            game_sub_status->value = 10;
        }
        if (final_pose->x < FIELD_X_1_2) {
            arah_target = 0;
        } else {
            arah_target = 180;
        }
        break;
    case 10:
        /**
         *? HOME
         * MENGARAHKAN BOLA
         * NEXT STATE -> 11
         */
        if (game_status->value - 128 == status_preparation_cornerkick_home) {
            if (motion->pivotMotion(final_pose->theta, 180)) {
                game_sub_status->value = 11;
            }
        } else if (game_status->value - 128 == status_preparation_goalkick_home) {
            if (motion->pivotMotion(arah_target, 180)) {
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
         * LEPAS BOLA
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
        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {

            motion->catchingBall(10, 10);
        } else {
            motion->catchingBall(190, 120);
        }

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
        pos_robot_buffer[0] = final_pose->x;
        pos_robot_buffer[1] = final_pose->y;
        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            // TODO: CEK APA SESUAI RULE
            if (motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
            }
        } else {
            // TODO: CEK APA SESUAI RULE
            if (motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
            }
        }

        //! KELUAR KARENA TIMEOUT 7 DETIK
        game_sub_status->timeout(99, 7);

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
            LogWithColor("magenta", "HADAP BOLA : %.2f\n", atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG);
            motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, motion->RobotAngletoBall(), 200, 120, 70, NORMAL_OBSTACLE);
        }

        // TODO: CEK APA SESUAI RULE
        if (motion->RobotDist2Point(pos_robot_buffer[0], pos_robot_buffer[1]) > 50) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(99, 7);

        break;
    case 3:
        /**
         ** PENALTY
         */
        break;
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

    if (game_sub_status->value != 100 && (*has_prep == 0 || *has_prep == 2)) {
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

    printf("TEST %d %.2f %.2f\n", obs.status, ball->distance, final_pose->theta);

    // MENENTUKAN STATE
    if (ball->is_caught == 2) {
        //? HAS BALL
        game_sub_sub_status.value = 2;
    } else if (ball->status > 0) {
        // FRIEND HAS BALL
        if (obs.status && ball->pos.x > FIELD_X_0 + 100 && ball->pos.x < FIELD_X_1 - 100 && ball->distance > ring_rebut_bola_min && ball->distance < ring_rebut_bola_max && ball->pos.y < FIELD_Y_1 - 125 && ball->pos.y > FIELD_Y_0 + 125 && (final_pose->theta > -15 || final_pose->theta < -165)) {
            //? ENEMY HAS BALL
            game_sub_sub_status.value = 1;
        } else {
            //? WILD BALL
            game_sub_sub_status.value = 4;
        }
    } else // TIDAK MELIHAT BOLA
    {
        //? ROAMING
        game_sub_sub_status.value = 0;
    }

    //=====================================================================================

    // Update posisi bola sebelum hilang, digunakan untuk roaming
    if (ball->status == 2) {
        *bola_terakhir_sebelum_hilang[0] = ball->pos.x;
        *bola_terakhir_sebelum_hilang[1] = ball->pos.y;
    }

    // Reset hysteresis paksa dribble
    if (game_sub_sub_status.value != 1) {
        stm_data->force_dribble_control = 0;
    }

    // Reset lock musuh
    if (ball->status == 2) {
        *enemy_locked = 0;
    }

    // Lock musuh agar bisa mengikuti posisi lawan, bukan bola
    if ((prev_sub_sub_state == 1 && game_sub_sub_status.value == 0) || (obs.status && game_sub_sub_status.value == 0)) {
        *enemy_locked = 1;
        *enemy_locked_obs = motion->obstacleChecking(motion->RobotAngletoPoint(obs.pos_x, obs.pos_y), 40,
            motion->RobotDist2Point(obs.pos_x, obs.pos_y) + 75, nearest_pixel);
    }

    //=====================================================================================

    printf("MAIN: %d\n", game_sub_sub_status.value);

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
        motion->manualMotion(0, 0, 0);
        break;
    case 5:
        EnemyLock(obs.pos_x, obs.pos_y);
        break;
    }

    prev_sub_sub_state = game_sub_sub_status.value;
    return;
}

void Defender::GameMainDouble() const
{
    game_sub_status->reentry(0, 0.04);

    static float pos_x_teman;
    static float pos_y_teman;

    static float pos_robot_buffer[2];
    pos_x_teman = shared_robot_data[bs_data->n_robot_att]->pose.x;
    pos_y_teman = shared_robot_data[bs_data->n_robot_att]->pose.y;

    static uint64_t pass_timer[2];
    static obstacle_checking_t obs;
    static obstacle_checking_t obs_tracking;
    static obstacle_checking_t obstacle;

    static float ball_buffer[2] = { 0, 0 }; // This is used for assign ball position buffer
    static float prev_ball_y;
    static float prev_ball_status_vision;

    static float counter_vel;
    static float ball_vel[2];
    static float x_intercept = 400;

    static float hadap_teman_invert;
    static float velocity_ball;

    // ===========PANDU
    static float defend_r_buffer = 350;
    // ===========PANDU

    obs = motion->obstacleChecking(motion->RobotAngletoBall(), 30, ball->distance + 50, NORMAL_OBSTACLE);

    // PERHITUNGAN KECEPATAN BOLA
    if (++counter_vel > 5) {
        // PERHITUNGAN TITIK
        //  printf("X Y INTERCEPT %.2f 100 || BALL REAL %.2f %.2f || PREV %.2f %.2f\n", x_intercept, ball->pos.x, ball->pos.y, *prev_ball[0], *prev_ball[1]);

        velocity_ball = pythagoras(ball->pos.x, ball->pos.y, *prev_ball[0], *prev_ball[1]);
        // arah_bola = atan2(ball->pos.y - *prev_ball[1], ball->pos.x - *prev_ball[0])  RAD2DEG;
        *prev_ball[0] = ball->pos.x;
        *prev_ball[1] = ball->pos.y;
        counter_vel = 0;
    }

    if (bs_data->style == 'C') {
        *region_by_style = FIELD_Y_1_2;
    } else {
        *region_by_style = FIELD_Y_3_4;
    }

    // CHECK OBSTACLE IN LAST BALL
    //--------------------------------------------------------------------------------

    if (*has_prep == 1) // This state is used for receiving ball from teammate (if the value is 1 then the robot has the ball)
    {
        game_sub_status->value = 100;
    }
    //--------------------------------------------------------------------------------

    static float jarak_teman = motion->RobotDist2Point(pos_x_teman, pos_y_teman);
    static float power_kick = 10;

    //* ATUR POWER KICK
    if (game_status->value - 128 == status_preparation_goalkick_home) {
        power_kick = 12.2; //
    } else if ((game_status->value - 128 == status_preparation_freekick_home)) {
        power_kick = 11;
    } else if ((game_status->value - 128 == status_preparation_cornerkick_home)) {
        power_kick = 11.3;
    } else if (game_status->value - 128 == status_preparation_throwin_home) {
        power_kick = 11.3;
    } else {
        power_kick = 8;
    }

    // power_kick = 8.3;

    printf("DEFENDER START DOUBLE : %d || %d || %.2f %.2f || %.2f %d\n", game_sub_status->value, *has_prep, pos_x_teman, pos_y_teman, jarak_teman, shared_robot_data[bs_data->n_robot_att]->robot_condition);

    //* ==============================================================================
    //*                              PREPARATION
    //* ==============================================================================

    switch (game_sub_status->value) {
    case 0:
        *robot_condition = 0;

        game_sub_status->resetUptimeTimeout();

        if (*has_prep == 0) // home
        {
            game_sub_status->value = 1;
        } else if (*has_prep == 2) // away
            game_sub_status->value = 2;
        else if (*has_prep == 3) // special
            game_sub_status->value = 3;
        else if (*has_prep == 4) // special
            game_sub_status->value = 4;

        *robot_condition = 0;

        break;
    //* ==============================================================================
    //*                              HOME
    //* ==============================================================================
    case 1:
        if (game_status->value - 128 == status_preparation_kickoff_home) {
            if (motion->motionAroundPointAngle(ball->pos.x, ball->pos.y, 10, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1_2), 60, 20, 90, NORMAL_OBSTACLE_FRIEND)) {
                // motion->manualMotion(0, 0, 0);
            }
        } else {
            if (motion->motionAroundBall(
                    atan2(shared_robot_data[bs_data->n_robot_att]->pose.y - ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x - ball->pos.x) RAD2DEG,
                    motion->RobotAngletoBall(),
                    65, 20, 90, NORMAL_OBSTACLE_FRIEND)) {
                // motion->manualMotion(0, 0, 0);
            }
        }

        if (bs_data->style != 'F') {
            game_sub_status->value = 11;
            break;
        }

        game_sub_status->timeout(11, 3);

        break;
    case 11:

        *robot_condition = 11;

        if (motion->passBall(pos_x_teman, pos_y_teman, power_kick, 1)) {
            game_sub_status->value = 210;
            game_sub_status->resetUptimeTimeout();
        } //
        break;
    case 210:
        //* KEMBALI KE CASE 1 JIKA BOLA MASIH DIPEGANG
        if (ball->is_caught == 2) {
            game_sub_status->value = 1;
        } else {
            if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
                game_sub_status->value = 99;
            }
        }

        motion->manualMotion(0, 0, 0);
        game_sub_status->timeout(99, 3);

        break;
    //* ==============================================================================
    //*                              AWAY
    //* ==============================================================================
    case 2:
        //* BERGERAK KE TENGAH LAPANGAN DAERAH SENDIRI

        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            if (motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 380, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
            }
        } else if (game_status->value - 128 == status_preparation_throwin_away) {
            if (motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 380, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
            }
            game_sub_status->timeout(200, 7);
        } else {
            // sisi kiri dari gawang sendiri
            if (motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, motion->RobotAngletoBall(), 400, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status->value = 20;
                pos_robot_buffer[0] = final_pose->x;
                pos_robot_buffer[1] = final_pose->y;
            }
            game_sub_status->timeout(200, 7);
        }

        if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
            if (game_status->value - 128 == status_preparation_cornerkick_away) {
                game_sub_status->value = 990;
                game_sub_status->resetUptimeTimeout();
            } else {
                game_sub_status->value = 99;
            }
        }

        break;
    case 20:
        printf("buff: %.2f %.2f || now: %.2f %.2f -> %.2f\n", pos_robot_buffer[0], pos_robot_buffer[1], final_pose->x, final_pose->y, pythagoras(final_pose->x, final_pose->y, pos_robot_buffer[0], pos_robot_buffer[1]));

        if (game_status->value - 128 == status_preparation_cornerkick_away) {
            motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 380, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        } else if (game_status->value - 128 == status_preparation_throwin_away) {
            motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_1_2 - 300), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 380, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
            game_sub_status->timeout(200, 7);
        } else {
            motion->motionAroundBall(atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG, motion->RobotAngletoBall(), 380, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
            game_sub_status->timeout(200, 7);
        }

        // Ketika attacker sudah memasuki mode main
        if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
            if (game_status->value - 128 == status_preparation_cornerkick_away) {
                game_sub_status->value = 990;
                game_sub_status->resetUptimeTimeout();
            } else {
                game_sub_status->value = 99;
            }
        }

        break;

    case 200:
        if (ball->distance < 125) {
            obstacle_checking_t obs_depan = motion->obstacleChecking(final_pose->theta, 25, 120, NORMAL_OBSTACLE);

            // Ketika bola dipinggir
            if ((ball->pos.x < FIELD_X_0 + 100 && final_pose->x < FIELD_X_0 + 170) || (ball->pos.x > FIELD_X_1 - 100 && final_pose->x > FIELD_X_1 - 170)) {
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
                    LogWithColor("blue", "Putari bola hadap lapangan\n");

                    // HARUSNYA DI STOP REFBOX
                    if (ball->pos.x < FIELD_X_0 || ball->pos.x > FIELD_X_1) {
                        motion->catchingBall(20, 50);
                    } else {
                        motion->motionAroundBall(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_3_4), motion->RobotAngletoBall(), 50, 150, 90, NORMAL_OBSTACLE);
                    }
                }
            }
            // Ketika bola didepan atau belakang
            else if ((ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 70 && final_pose->y > FIELD_Y_1 - 110)) {
                LogWithColor("cyan", "KEJAR PELAN SEKALI\n");
                motion->catchingBall(20, 50);
            } else {
                if (obs_depan.status) {
                    LogWithColor("blue", "KEJAR LAMBAT\n");
                    motion->catchingBall(160, 150);
                } else {
                    if (ball->distance < 300) {
                        LogWithColor("magenta", "KEJAR CEPAT\n");
                        motion->catchingBall(200, 150, LANGSUNG_AMBIL);
                    } else {
                        LogWithColor("red", "KEJAR CEPAT SEKALI\n");
                        motion->catchingBall(240, 180, LANGSUNG_AMBIL);
                    }
                }
            }
        } else if (motion->RobotDist2Point(pos_x_teman, pos_y_teman) < motion->RobotDist2Point(ball->pos.x, ball->pos.y)) {
            if (motion->motionToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 100, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status->value = 201;
            }

            // Safety jika tidak sengaja memegang bola
            if (ball->is_caught == 2) {
                game_sub_status->value = 201;
            }
        } else {
            if (motion->motionToPoint(ball->pos.x, ball->pos.y, motion->RobotAngletoBall(), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
                game_sub_status->value = 201;
            }

            // Safety jika tidak sengaja memegang bola
            if (ball->is_caught == 2) {
                game_sub_status->value = 201;
            }
        }

        // Safety jika tidak sengaja memegang bola
        if (ball->is_caught == 2) {
            game_sub_status->value = 201;
        }

        if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(99, 7 + 12);

        break;

    case 201:
        if (ball->is_caught == 0) {
            game_sub_status->value = 200;
        }

        // Mencari posisi untuk mengumpan bola
        if (motion->RobotDist2Point(*titik_final_positioning[0], *titik_final_positioning[1]) < 100) {
            motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 50, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        } else {
            motion->motionToPoint(*titik_final_positioning[0], *titik_final_positioning[1], motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 100, 50, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        }

        // Ketika sudah dekat
        if (motion->RobotDist2Point(*titik_final_positioning[0], *titik_final_positioning[1]) < 80) {
            game_sub_status->value = 202;
        }

        // Ketika kosong
        {
            obstacle_checking_t obs_ke_teman = motion->obstacleChecking(motion->RobotAngletoPoint(pos_x_teman, pos_y_teman), 25, motion->RobotDist2Point(pos_x_teman, pos_y_teman) + 50, NORMAL_OBSTACLE);

            if (!obs_ke_teman.status) {
                game_sub_status->value = 202;
            }
        }

        if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
            game_sub_status->value = 99;
        }

        break;

    case 202:

        *robot_condition = 11;

        if (motion->passBall(pos_x_teman, pos_y_teman, 8, 5)) {
            game_sub_status->value = 203;
            game_sub_status->resetUptimeTimeout();
        } //
        break;

    case 203:
        //* KEMBALI KE CASE 202 JIKA BOLA MASIH DIPEGANG
        if (ball->is_caught == 2) {
            game_sub_status->value = 202;
        } else {
            if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
                game_sub_status->value = 99;
            }
        }

        motion->manualMotion(0, 0, 0);
        game_sub_status->timeout(99, 3);

        break;

    case 3:
        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5 + 50, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL);

        if (shared_robot_data[bs_data->n_robot_att]->robot_condition == 99) {
            game_sub_status->value = 99;
        }
        break;

    case 4:
        motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, 20, motion->RobotAngletoBall(), 250, 70, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);

        // Ketika bola sudah ditendang
        if (ball->status > 0 && pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_1_2) > 250 && ball->pos.y > FIELD_Y_0 + 50 && ball->pos.y < FIELD_Y_1_4) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(200, 7);
        break;
    case 990:
        motion->motionAroundBall(atan2(ball->pos.y - (FIELD_Y_0 - 200), ball->pos.x - FIELD_X_1_2) RAD2DEG, motion->RobotAngletoBall(), 70, 180, 80, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);

        if (deltaAngle(motion->RobotAngletoBall(), atan2(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG) < 10) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(99, 5);

        break;

    case 999:
        if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 120, 70, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }

        if (ball->pos.y > FIELD_Y_1_3) {
            game_sub_status->value = 99;
        }

        game_sub_status->timeout(99, 7);
        break;

    case 99:
        *has_prep = 1;

        game_sub_status->value = 100;
        break;
    }

    if (game_sub_status->value != 100 && (*has_prep == 0 || *has_prep == 2 || *has_prep == 3 || *has_prep == 4)) {
        return;
    }

    //======================================================================================
    *has_prep = 1;
    *robot_condition = 99;

    static MachineState game_sub_sub_status;
    game_sub_sub_status.reentry(0, 0.4);

    //* JIKA MEMEGANG BOLA MASUK case 10
    if (ball->is_caught > 0) {
        game_sub_sub_status.value = 10; // LANGSUNG TENDANG KE GAWANG LAWAN
    }

    //======================================================================================

    // MENCATAT BOLA TERAKHIR
    if (ball->status == 2) {
        *bola_terakhir_sebelum_hilang[0] = ball->pos.x;
        *bola_terakhir_sebelum_hilang[1] = ball->pos.y;
    }
    // obs = motion->obstacleChecking( motion->RobotAngletoPoint(bola_terakhir_sebelum_hilang[0], bola_terakhir_sebelum_hilang[1]), 30, ball->distance + 100, NORMAL_OBSTACLE);

    if (ball->distance < 200) {
        obs = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance + 50, NORMAL_OBSTACLE);
    }

    if (ball->status == 1) {
        *enemy_locked = 0;
    }

    // static uint8_t game_sub_sub_status.value = 0;
    static uint8_t prev_sub_sub_state = 0;
    static uint16_t cntr = 0;
    static uint8_t flag_transisi = 0;
    static uint64_t time_transisi = getTimeNowMSec();

    static float jarak_bola_buffer = ball->distance;
    float sudut_teman_ke_bola = atan2(ball->pos.y - shared_robot_data[bs_data->n_robot_att]->pose.y, ball->pos.x - shared_robot_data[bs_data->n_robot_att]->pose.x) RAD2DEG;
    // static float sudut_teman_ke_bola = atan2(ball->pos.y - 600, ball->pos.x - 400)  RAD2DEG;
    float sudut_aku_ke_bola = motion->RobotAngletoBall();

    static float dynamic_limit = 80;
    static float x_arching = 400;

    printf("SUDUT %.2f %.2f ||%d TEMAN %.2f %.2f \n", sudut_aku_ke_bola, sudut_teman_ke_bola, bs_data->n_robot_att, shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y);
    // printf("BALL STATUS %d  Y =%.2f\n", ball->status, ball->pos.y);
    if (ball->is_caught > 0) {
        // printf("MASUK MEGANG BOLA\n");
        game_sub_sub_status.value = 2;
    } else if (bs_data->n_has_ball > 0 && ball->is_caught == 0) {
        game_sub_sub_status.value = 3;
    } else if (ball->status > 0) {
        // PENENTU TITIK ARCHING X
        //  printf("MASUK SINII CUYY\n");
        // if (ball->pos.x > FIELD_X_1_2)
        // {
        //     x_arching = FIELD_X_1_2 - 120;
        // }
        // else if (ball->pos.x < FIELD_X_1_2)
        // {
        //     x_arching = FIELD_X_1_2 + 120;
        // }

        x_arching = FIELD_X_1_2;

        if (ball->pos.y < FIELD_Y_1_2 && final_pose->y < FIELD_Y_2_3 && ball->pos.y < shared_robot_data[bs_data->n_robot_att]->pose.y && final_pose->y < shared_robot_data[bs_data->n_robot_att]->pose.y && fabs(velocity_ball) > 10 && !obs.status) {

            LogWithColor("yellow", "INTERCEP CASE\n");
            game_sub_sub_status.value = 4;
            jarak_bola_buffer = 0;
            defend_r_buffer = 350;
        } else if (ball->pos.y < FIELD_Y_1_4 + 10) {
            game_sub_sub_status.value = 5;
            jarak_bola_buffer = 0;
            defend_r_buffer = 350;
            LogWithColor("yellow", "KEJAR BOLA LANGSUNG\n");
        }

        else if (prev_ball_y > *region_by_style && ball->pos.y < *region_by_style) {
            // PERALIHAN REGION 3 KE 2
            jarak_bola_buffer = ball->distance;

            defend_r_buffer = 350;
            LogWithColor("magenta", "PERALIHAN REGION 3-2 state:%d ball:%.2f  prev_ball:%.2f || %.2f\n",
                game_sub_sub_status.value, ball->pos.y, prev_ball_y, jarak_bola_buffer);
            game_sub_sub_status.value = 1;
        } else if (prev_ball_y < *region_by_style && ball->pos.y > *region_by_style) {
            // PERALIHAN REGION 2 KE 3
            LogWithColor("cyan", "PERALIHAN REGION 2-3 state:%d ball:%.2f  prev_ball:%.2f\n", game_sub_sub_status.value, ball->pos.y, prev_ball_y);
            jarak_bola_buffer = 0;
            defend_r_buffer = 350;
            game_sub_sub_status.value = 1;
        }
        // else if (shared_robot_data[bs_data->n_robot_att].pos_y < ball->pos.y) {
        //     defend_r_buffer = 350;
        //     game_sub_sub_status.value = 1;

        // }
        // else if (!obs.status && ball->pos.y < FIELD_Y_1_2 && n_has_ball == 0 && Pythagoras(shared_robot_data[bs_data->n_robot_att].pos_x, shared_robot_data[bs_data->n_robot_att].pos_y, ball->pos.x, ball->pos.y) > 300)
        // {
        //     game_sub_sub_status.value = 5;
        // }

        else if (ball->pos.y > FIELD_Y_1_4 && ball->pos.y < *region_by_style) {
            // Ketika teman masih jauh
            if (deltaAngle(sudut_aku_ke_bola, sudut_teman_ke_bola) > 60) {
                dynamic_limit -= 5.2;
                dynamic_limit = fmaxf(120, dynamic_limit);
            } else if (deltaAngle(sudut_aku_ke_bola, sudut_teman_ke_bola) < 100 && pythagoras(ball->pos.x, ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y) < 300) {
                dynamic_limit += 12;
                dynamic_limit = fminf(300, dynamic_limit);
            } else if (pythagoras(ball->pos.x, ball->pos.y, shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y) < 160) {
                dynamic_limit += 9;
                dynamic_limit = fminf(300, dynamic_limit);
            } else {
                dynamic_limit -= 5.2;
                dynamic_limit = fmaxf(120, dynamic_limit);
            }

            jarak_bola_buffer -= 1;
            jarak_bola_buffer = fmaxf(dynamic_limit, fminf(ball->distance, jarak_bola_buffer));

            float jarak_bola_ke_gawang = pythagoras(x_arching, FIELD_Y_0, ball->pos.x, ball->pos.y);

            defend_r_buffer = jarak_bola_ke_gawang - jarak_bola_buffer;

            LogWithColor("green", " Jarak %.2f | defend %.2f | limit %.2f | BALL REAL %.2f %.2f\n",
                jarak_bola_buffer, defend_r_buffer, dynamic_limit, ball->pos.x, ball->pos.y);

            // Jika posisi saya lebih depan dari teman
            // if (final_pose->y < shared_robot_data[bs_data->n_robot_att]->pose.y)
            // {
            //     game_sub_sub_status.value = 10;
            // }
            // else
            // {
            game_sub_sub_status.value = 1;
            // }
        } else if (ball->pos.y > *region_by_style) {

            game_sub_sub_status.value = 1;
            jarak_bola_buffer = 0;
            defend_r_buffer = 350;
        } else {
            game_sub_sub_status.value = 1;
        }
    } else {

        jarak_bola_buffer = 0;
        defend_r_buffer = 350;
        game_sub_sub_status.value = 0;
    }

    printf("MAIN: %d\n", game_sub_sub_status.value);

    switch (game_sub_sub_status.value) {
    case 0:
        if (prev_sub_sub_state == 4)
            Roaming(10);
        else
            Roaming(2);
        break;
    case 1:
        StandBy(defend_r_buffer, x_arching);
        break;
    case 2:
        HasBall();
        break;
    case 3:
        FriendHasBall(); // When the ball is held by friend
        break;
    case 4:
        Intercept();
        // CatchBallAdvance(0.6, 250, 1);

        break;
    case 5:
        // HasBall();
        // CatchBallAdvance(0.6, 200);
        LogWithColor("yellow", "KEJAR BOLA LANGSUNG case 5\n");

        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 70 && final_pose->y < FIELD_Y_0 + 130) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
            motion->catchingBall(20, 40);
        } else {
            motion->catchingBall(150, 100);
        }
        // MotionToPoint(x_intercept, 100, RobotAngletoBall(), 250, 90);
        // WildBall();
        break;
    }

    prev_ball_y = ball->pos.y;
    prev_sub_sub_state = game_sub_sub_status.value;
    prev_ball_status_vision = ball->status;

    *prev_n_has_ball = bs_data->n_has_ball;

    return;

    //==============================================

    switch (game_sub_sub_status.value) {
    case 0:
        stm_data->global_kicker_position = 800;
        if (motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_4, 90, 100, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL)) {
            motion->manualMotion(0, 0, 0);
        }

        if (ball->status > 0) {
            game_sub_sub_status.value = 1;
        }
        break;

    case 1:
        stm_data->global_kicker_position = 800;

        if (ball->status == 0) {
            game_sub_sub_status.value = 0;
            break;
        }

        if (ball->pos.y > FIELD_Y_1_2 + 100) {
            motion->ruckigToPoint(FIELD_X_1_2, FIELD_Y_1_4, 90, 100, 100, NORMAL_OBSTACLE);
        } else if (ball->pos.y > FIELD_Y_0) {
            if (pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_0) > 75) {
                motion->motionAroundBall(
                    atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2) RAD2DEG,
                    ball->angle,
                    pythagoras(ball->pos.x, ball->pos.y, FIELD_X_1_2, FIELD_Y_0) - 200, 120, 100);
            } else {
                motion->manualMotion(0, 0, 0);
            }
        }

        if (ball->distance < 100) {
            game_sub_sub_status.value = 2;
        }

        break;

    case 2:
        stm_data->global_kicker_position = 800;

        if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
            if (motion->catchingBall(10, 10)) {
                game_sub_sub_status.value = 10;
            }
        } else {

            if (motion->catchingBall(120, 90)) {
                game_sub_sub_status.value = 10;
            }
        }
        break;

    case 10:
        // if (motion->passBall(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, 10, 5))
        // {
        //     game_sub_sub_status.value = 0;
        // }

        //* TENDANG KE ARAH TENGAH GAWANG LAWAN
        if (deltaAngle(final_pose->theta, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1)) > 5) {
            motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_2, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 5, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
            stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
            break;
        }

        if (ball->is_caught > 0) {
            kickerControl(stm_data, KICKER_POSITION_AIM, 37, motion->AutomaticKickerPos(40));
        }

        break;
    }
}

void Defender::GameMainTriple() const { }
void Defender::GameMainQuadruple() const { }

void Defender::HasBall() const
{
    static float target_x;
    static float target_y;
    static float buffer_x;
    static float buffer_y;

    // static float pos_buffer_robot_has_ball[2];

    static obstacle_checking_t obstacle;
    static obstacle_checking_t obs_ke_teman;
    static uint16_t counter_menggiring_sebelum_tendang = 0;
    static float pos_x_sebelum_gerak_maju = 0;
    static float pos_y_sebelum_gerak_maju = 0;
    static float kecepatan_translasi_maju = 100;
    static uint8_t prev_lock_goal_kiri_atau_kanan = 0; // kiri 1, kanan 2

    static MachineState has_ball_state_assist;

    static target_goal_t buffer_target_goal;

    static float pos_buffer[2];
    static int8_t n_robot_saya = atoi(getenv("ROBOT"));

    has_ball_state_assist.reentry(0, 0.4);

    //----------------
    static float maju_x, maju_y;

    static const float titik_target_x[4] = { 130, 250, 550, 670 };
    static const float titik_target_y[4] = { 950, 750, 750, 950 };
    static int idx_target;
    static int init_idx;

    if (final_pose->x > FIELD_X_0 + 20 && final_pose->x < FIELD_X_1_2)
        stm_data->long_dribble_control = 0;
    else if (final_pose->x < FIELD_X_1 - 20 && final_pose->x > FIELD_X_1_2)
        stm_data->long_dribble_control = 0;
    else if (final_pose->x < FIELD_X_0 + 20)
        stm_data->long_dribble_control = 1;
    else if (final_pose->x > FIELD_X_1 - 20)
        stm_data->long_dribble_control = 1;

    LogWithColor("green", "STATE ASSIST %d || JARAK %.2f\n", has_ball_state_assist.value, motion->RobotDist2Point(*pos_buffer_robot_has_ball[0], *pos_buffer_robot_has_ball[1]));

    switch (has_ball_state_assist.value) {
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

        *pos_buffer_robot_has_ball[0] = final_pose->x;
        *pos_buffer_robot_has_ball[1] = final_pose->y;

        buffer_x = titik_target_x[idx_target];
        buffer_y = titik_target_y[idx_target];
        target_x = titik_target_x[idx_target];
        target_y = titik_target_y[idx_target];

        pos_buffer[0] = final_pose->x;
        pos_buffer[1] = final_pose->y;

        if (*prev_n_has_ball == n_robot_saya)
            has_ball_state_assist.value = 10;
        else
            has_ball_state_assist.value = 1;

        has_ball_state_assist.resetUptimeTimeout();

        counter_menggiring_sebelum_tendang = 0;

        break;

    case 1:
        /**
         * CEK KONDISI ROBOT
         * JIKA TERLALU KANAN KE KIRI DULU 60 CM
         * JIKA TERLALU KIRI KE KANAN DULU 60 CM
         * JIKA DI HADANG OBSTACLE MUNDUR DULU 40 CM
         */
        if (final_pose->x < FIELD_X_0 + 100)
            has_ball_state_assist.value = 2;
        else if (final_pose->x > FIELD_X_1 - 100)
            has_ball_state_assist.value = 3;
        else {
            obstacle = motion->obstacleChecking(final_pose->theta, 25, 120, NORMAL_OBSTACLE);
            if (obstacle.status)
                has_ball_state_assist.value = 4;
            else {
                has_ball_state_assist.value = 10;
                has_ball_state_assist.resetUptimeTimeout();
            }
        }
        break;

    case 2:
        // GERAK KE KANAN 60 CM
        if (motion->motionToPoint(pos_buffer[0] + 60, final_pose->y, final_pose->theta, 120, 90))
            has_ball_state_assist.value = 10;

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 60)
            has_ball_state_assist.value = 10;

        has_ball_state_assist.resetUptimeTimeout();
        break;

    case 3:
        // GERAK KE KIRI 60 CM
        if (motion->motionToPoint(pos_buffer[0] - 60, final_pose->y, final_pose->theta, 120, 90))
            has_ball_state_assist.value = 10;

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 60)
            has_ball_state_assist.value = 10;

        has_ball_state_assist.resetUptimeTimeout();
        break;

    case 4:
        // MUNDUR SEJAUH 40 CM KARENA OBS DI JARAK 120 CM
        motion->manualMotion(0, -150, 0);

        if (motion->RobotDist2Point(pos_buffer[0], pos_buffer[1]) > 40)
            has_ball_state_assist.value = 10;

        *hisap_dribble_lama = 0;
        has_ball_state_assist.resetUptimeTimeout();
        break;

    case 10: {
        /**
         * JALAN KE BUFFER SAMBIL CEK KONDISI OBSTACLE
         * STYLE B LANGSUNG PINDAH CASE LANGSUNG HADAP KE GAWANG
         * STYLE E MENGGUNAKAN TITIK FINAL SEBAGAI TARGET BARU
         * NEXT STATE -> 20 GIRING GOAL
         * NEXT STATE -> 200 PASS TO ATTACKER
         */
        stm_data->long_dribble_control = 0;

        if (bs_data->style == 'B') {
            /**
             * ! =========================================
             * ! STYLE B
             * ! TARGET GIRING -> TARGET FINAL POSITIONING
             * ! =========================================
             */
            has_ball_state_assist.value = 20;
            break;
        }

        float kecepatan_memindah_target = 0;

        // JALAN MENUJU BUFFER DAN JUGA CEK OBSTACLE KE ARAH GAWANG
        obstacle = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 250, NORMAL_OBSTACLE);
        obs_ke_teman = motion->obstacleChecking(motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 10, 250, NORMAL_OBSTACLE);

        // Menentukan ketinggian penendang
        if (final_pose->y < FIELD_Y_1_2) {
            stm_data->global_kicker_position = 200;
        } else {
            stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        }

        // LogWithColor("yellow", "buffer: %.2f %.2f || target: %.2f %.2f || pose: %.2f %.2f\n"
        //     , buffer_x, buffer_y, target_x, target_y, final_pose->x, final_pose->y);

        // 2024
        float sudut_saya_ke_gawang = motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1);
        float sudut_teman_ke_gawang = atan2f(FIELD_Y_1 - shared_robot_data[bs_data->n_robot_att]->pose.y, FIELD_X_1_2 - shared_robot_data[bs_data->n_robot_att]->pose.x) RAD2DEG;

        float selisih_sudut = deltaAngle(sudut_saya_ke_gawang, sudut_teman_ke_gawang);

        float jarak_saya_ke_teman = motion->RobotDist2Point(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y);

        float error_posisi = jarak_saya_ke_teman * selisih_sudut DEG2RAD;

        // fabs(error_posisi) < 100;

        // MOTION
        if (final_pose->y < 900)
            motion->ruckigToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 120, 110, NORMAL_OBSTACLE | INVERT_OBS);
        else
            motion->motionToPoint(buffer_x, buffer_y, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 120, 110, NORMAL_OBSTACLE | INVERT_OBS);

        // CEK OBSTACLE
        if (obstacle.status && motion->RobotDist2Point(titik_target_x[idx_target], titik_target_y[idx_target]) < 70 && bs_data->style != 'E') {
            idx_target++;

            // DEPAN KIRI, BELAKANG KIRI | BELAKANG KANAN, DEPAN KANAN
            //{ 130,        250          | 550,            670 };
            //{ 950,        750          | 750,            950 };
            // DEPAN KIRI <-> BELAKANG KIRI
            // DEPAN KANAN <-> BELAKANG KANAN

            if (init_idx <= 1) {
                if (idx_target > 1)
                    idx_target = 0;
            } else if (init_idx >= 2) {
                if (idx_target > 3)
                    idx_target = 2;
            }

            target_x = titik_target_x[idx_target];
            target_y = titik_target_y[idx_target];
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
        if (final_pose->y > 600 && final_pose->y < 900 && // 1000 aslinya terlalu mepet
            final_pose->x > 100 && final_pose->x < 700 && !obstacle.status) {
            has_ball_state_assist.value = 20;
            LogWithColor("red", "KELUAR KARENA DIRASA GK ADA OBSTACLE -------------------\n");
        }

        //! KELUAR KARENA GIRING LEBIH DARI 3M
        if (motion->RobotDist2Point(*pos_buffer_robot_has_ball[0], *pos_buffer_robot_has_ball[1]) > 220) {
            if (obstacle.status && final_pose->y > FIELD_Y_1_2 - 150 && bs_data->style != 'C' && bs_data->style != 'D' /*&& final_pose->y < FIELD_Y_1_2 + 250*/) {
                *robot_condition = is_passing_inside_state;
                has_ball_state_assist.value = 200;
            } else {
                has_ball_state_assist.value = 20;
            }
            LogWithColor("red", "KELUAR KARENA 300 CM ----------------------------------\n");
        }

        kecepatan_memindah_target = 1;

        float dx = target_x - buffer_x;
        float dy = target_y - buffer_y;
        float r = fminf(kecepatan_memindah_target, sqrtf(dx * dx + dy * dy));
        float a = atan2f(dy, dx);

        buffer_x += r * cosf(a);
        buffer_y += r * sinf(a);

        counter_menggiring_sebelum_tendang = 0;

        //! KELUAR KARENA TIMEOUT
        has_ball_state_assist.timeout(20, 10);

        break;
    }
    case 20: {

        /**
         * MENGHADAPKAN KE GAWANG
         * JIKA ERROR KECIL -> RobotAngletoPoint
         * JIKA ERROR BESAR -> pivotMotion -> RobotAngletoPoint
         */
        stm_data->force_dribble_control = 1;

        if (final_pose->y < FIELD_Y_1_2) {
            stm_data->global_kicker_position = 200;
        } else {
            stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        }

        obstacle_checking_t obs_gawang;
        if (final_pose->y > 900) {
            obs_gawang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 25, 100, NORMAL_OBSTACLE);
        } else {
            obs_gawang = motion->obstacleChecking(motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 90, 100, NORMAL_OBSTACLE);
        }

        // Safety ketika terlalu depan
        if (final_pose->y > 940) {
            if (motion->motionToPoint(FIELD_X_1_2, FIELD_Y_3_4, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 80, 90)) {
                has_ball_state_assist.value = 30;
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

            has_ball_state_assist.value = 30;
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

        if (motion->motionAngleControlPivot(vx_local * 0.3, vy_local, motion->RobotAngletoPoint(FIELD_X_1_2, FIELD_Y_1), 160)) {
            buffer_target_goal = motion->sudut_robot_ke_gawang_lawan();
            LogWithColor("red", "TRANSISI GOAL PAKE TOLERANSI ERROR BESAR || target %d -> %.2f %.2f || min %.2f %.2f || max %.2f %.2f\n",
                buffer_target_goal.status_gawang_kosong,
                buffer_target_goal.goal_tengah[0], buffer_target_goal.goal_tengah[1],
                buffer_target_goal.goal_kiri[0], buffer_target_goal.goal_kiri[1],
                buffer_target_goal.goal_kanan[0], buffer_target_goal.goal_kanan[1]);

            // float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(330, 1200) : motion->RobotAngletoPoint(460, 1200);

            has_ball_state_assist.value = 30;

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

        if (final_pose->y < FIELD_Y_1_2) {
            stm_data->global_kicker_position = 200;
        } else {
            stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        }

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
            has_ball_state_assist.value = 40;
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
        //     has_ball_state_assist.value = 40;
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
            has_ball_state_assist.value = 40;
            prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
        }
        // if (motion->motionAngleControlPivot(vx_local * 0.3, vy_local, target_sudut_shot, 120)) {
        //     LogWithColor("red", "SHOT PAKAI TOLERANSI BIASA %.2f %.2f || target %.2f || saya %.2f\n",
        //         delta_saya_ke_minimum,
        //         delta_saya_ke_maksimum,
        //         target_sudut_shot,
        //         final_pose->theta);
        //     has_ball_state_assist.value = 40;
        // }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_sebelum_gerak_maju, pos_y_sebelum_gerak_maju) > 50) {
            LogWithColor("red", "SHOT KARENA GERAK TERLALU MAJU %.2f %.2f || target %.2f || saya %.2f\n",
                delta_saya_ke_minimum,
                delta_saya_ke_maksimum,
                target_sudut_shot,
                final_pose->theta);
            kecepatan_translasi_maju = 100;
            has_ball_state_assist.value = 40;
            prev_lock_goal_kiri_atau_kanan = *lock_goal_kiri_atau_kanan;
        }

        counter_menggiring_sebelum_tendang = 0;

        break;
    }
    case 40: {
        /**
         * TENDANG GOAL DENGAN MENGARAHKAN KE 350 ATAU 435 (SEBENTAR)
         */
        if (final_pose->y < FIELD_Y_1_2) {
            stm_data->global_kicker_position = 200;
        } else {
            stm_data->global_kicker_position = motion->AutomaticKickerPos(40);
        }
        float target_sudut_shot = buffer_target_goal.goal_tengah[0] < 400 ? motion->RobotAngletoPoint(335, 1200) : motion->RobotAngletoPoint(455, 1200);

        motion->motionToPoint(final_pose->x, final_pose->y, target_sudut_shot, 50, 90, NORMAL_OBSTACLE);

        if (ball->is_caught > 0) {
            kickerControl(stm_data, NO_AIM, 35);
        } else {
            has_ball_state_assist.value = 50;
            has_ball_state_assist.resetUptimeTimeout();
        }

        break;
    }
    case 50:
        motion->manualMotion(0, 0, 0);
        break;
    case 200:
        printf("PASS BALL UMPAN \n");
        if (motion->passBall(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y, 9, 6)) {
            has_ball_state_assist.value = 50;
            has_ball_state_assist.resetUptimeTimeout();
        }

        break;
    }
}

void Defender::FriendHasBall() const
{
    static MachineState frined_has_ball_state;
    frined_has_ball_state.reentry(0, 5);

    LogWithColor("cyan", "FriendHasBall: %d || %.2f %.2f\n", frined_has_ball_state.value, shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y);

    // printf("FriendHasBall: %d || %.2f %.2f\n", frined_has_ball_state.value, shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y);

    switch (frined_has_ball_state.value) {
    case 0:
        frined_has_ball_state.resetUptimeTimeout();
        frined_has_ball_state.value = 1;
        break;

    case 1:
        motion->manualMotion(0, 0, 0);
        frined_has_ball_state.timeout(2, 1.5);
        break;

    case 2:
        // if (shared_robot_data[bs_data->n_robot_att]->pose.x > FIELD_X_1_2)
        // {
        //     motion->motionToPoint(FIELD_X_1_2 - 200, FIELD_Y_1_2 - 150, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 100, 80, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        // }
        // else
        // {
        //     motion->motionToPoint(FIELD_X_1_2 + 200, FIELD_Y_1_2 - 150, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 100, 80, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        // }
        motion->motionToPoint(FIELD_X_1_2, FIELD_X_1_5 + 20, motion->RobotAngletoPoint(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y), 120, 90, NORMAL_OBSTACLE_FRIEND | NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        break;
    }
}
void Defender::EnemyHasBall() const
{
    static MachineState enemy_has_ball_state;
    enemy_has_ball_state.reentry(0, 0.04);

    static float jarak_saya_ke_lawan;
    static obstacle_checking_t lawan;
    static float sudut_hadap_lawan;
    static float buffer_lawan[2];
    static float sudut_gawang_ke_bola = 90;

    lawan = motion->obstacleChecking(motion->RobotAngletoBall(), 25, ball->distance, NORMAL_OBSTACLE);
    jarak_saya_ke_lawan = motion->RobotDist2Point(lawan.pos_x, lawan.pos_y);

    LogWithColor("yellow", "ENEMY HASSSS BALLLLLL: %d obs: %.2f %.2f || jarak: %.2f %.2f\n",
        enemy_has_ball_state.value, lawan.pos_x, lawan.pos_y,
        jarak_saya_ke_lawan, ball->distance);

    switch (enemy_has_ball_state.value) {
    case 0:
        if (bs_data->style == 'C') {
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
            if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
                motion->catchingBall(10, 10);
            } else {
                motion->catchingBall(120, 90);
            }
        }
        break;

    case 10:
        sudut_hadap_lawan = atan2(ball->pos.y - lawan.pos_y, ball->pos.x - lawan.pos_x) RAD2DEG;

        if (motion->motionAroundBall(sudut_hadap_lawan - 180, motion->RobotAngletoBall(), 50, 100, 90))
            enemy_has_ball_state.value = 0;

        if (jarak_saya_ke_lawan > ball->distance)
            enemy_has_ball_state.value = 0;

        enemy_has_ball_state.timeout(0, 3);
        break;

    case 20:
        sudut_gawang_ke_bola = atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG;

        motion->motionAroundBall(sudut_gawang_ke_bola, sudut_gawang_ke_bola, 80, 120, 90);
        break;
    }
}
void Defender::WildBall() const
{
    static float sudut_robot_ke_bola;
    static float sudut_gawang_ke_bola;

    static MachineState wild_ball_state;
    wild_ball_state.reentry(0, 0.05);

    LogWithColor("green", "WILDBALL %d theta %.2f\n", wild_ball_state.value, final_pose->theta);

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
        if (ball->distance < 135 && ball->distance > 100 && final_pose->y > ball->pos.y && ball->pos.x > FIELD_X_0 + 30 && ball->pos.y < FIELD_X_1 - 30 && ball->pos.y > FIELD_Y_1_4) {
            wild_ball_state.value = 1;
            wild_ball_state.resetUptimeTimeout();
        }
        break;

    case 1:
        /**
         * RESET STATE KE 0
         */
        sudut_gawang_ke_bola = atan2(ball->pos.y - 0, ball->pos.x - 400) RAD2DEG;
        sudut_robot_ke_bola = atan2(ball->pos.y - final_pose->y, ball->pos.x - final_pose->x) RAD2DEG;

        if (motion->motionAroundBall(sudut_gawang_ke_bola, sudut_gawang_ke_bola, 75, 150, 90)) {
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
    }
}
void Defender::Roaming(int default_state) const
{
    static MachineState roaming_state_assist;

    LogWithColor("cyan", "ROAMINGGG roaming_state_ass %d || %d (%d)\n", roaming_state_assist.value, default_state, *enemy_locked);
    roaming_state_assist.reentry(default_state, 2);

    static float target_sudut_patroli = 0;
    static float sudut_teman_ke_obs;
    static float sudut_aku_ke_obs;
    static float buffer_obs[2];

    target_sudut_patroli = atan2(final_pose->y - FIELD_Y_1_2, final_pose->x - FIELD_X_1_2) RAD2DEG - 180;

    if (*enemy_locked) {

        *enemy_locked_obs = motion->obstacleChecking(motion->RobotAngletoPoint(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y), 40,
            motion->RobotDist2Point(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y) + 75, NORMAL_OBSTACLE);
    }

    if (motion->RobotDist2Point(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y) > 200) {
        *enemy_locked = 0;
    }

    if (*enemy_locked == 1) {
        roaming_state_assist.value = 6;
    }

    switch (roaming_state_assist.value) {
    case 0:
        // MENENTUKAN STATE
        // roaming_state_assist.value = default_state;

        // 2024
        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5 + 20, 90, 160, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);

        break;

        //==================================================================================

        break;
    case 10:
        /**
         * STATE SETELAH WILD BALL
         */
        roaming_state_assist.resetUptimeTimeout();
        if (*bola_terakhir_sebelum_hilang[0] == 0 && *bola_terakhir_sebelum_hilang[1] == 0)
            roaming_state_assist.value = 2;
        else
            roaming_state_assist.value = 1;
        break;
    case 1:
        /**
         * MENUJU LOKASI TERAKHIR BOLA HILANG
         */
        LogWithColor("green", "TIME OUT ROAMING STATE 1\n");
        motion->motionToPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1], motion->RobotAngletoPoint(*bola_terakhir_sebelum_hilang[0], *bola_terakhir_sebelum_hilang[1]), 120, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        roaming_state_assist.timeout(2, 2);
        break;

        //==================================================================================
        /**
         * BERGERAK LINEAR KANAN KIRI
         * 21 -> KIRI BAWAH
         * 22 -> KANAN BAWAH
         */

    case 2:
        if (*bola_terakhir_sebelum_hilang[0] < FIELD_X_1_2) {
            roaming_state_assist.value = 21;
        } else if (*bola_terakhir_sebelum_hilang[0] > FIELD_X_1_2) {
            roaming_state_assist.value = 22;
        }

        break;
    case 21:
        if (motion->motionToPoint(FIELD_X_1_2 - 250, FIELD_Y_1_4, motion->RobotAngletoPoint(400, 600), 120, 75, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            roaming_state_assist.value = 22;
        }

        if (motion->RobotDist2Point(FIELD_X_1_2 - 250, FIELD_Y_1_4) < 100) {
            roaming_state_assist.value = 22;
        }
        break;
    case 22:
        if (motion->motionToPoint(FIELD_X_1_2 + 250, FIELD_Y_1_4, motion->RobotAngletoPoint(400, 600), 120, 75, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND)) {
            roaming_state_assist.value = 21;
        }
        if (motion->RobotDist2Point(FIELD_X_1_2 + 250, FIELD_Y_1_4) < 100) {
            roaming_state_assist.value = 21;
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
            roaming_state_assist.value = 2;
            break;
        }

        // sudut_teman_ke_obs = atan2(*enemy_locked_obs.pos_y - robot_data[n_robot_att].pos_y, *enemy_locked_obs.pos_x - robot_data[n_robot_att].pos_x) RAD2DEG;
        // sudut_aku_ke_obs = RobotAngletoPoint(*enemy_locked_obs.pos_x, *enemy_locked_obs.pos_y);

        if (motion->RobotDist2Point(shared_robot_data[bs_data->n_robot_att]->pose.x, shared_robot_data[bs_data->n_robot_att]->pose.y) < 200) {
            buffer_obs[0] = enemy_locked_obs->pos_x;
            buffer_obs[1] = enemy_locked_obs->pos_y;
            roaming_state_assist.value = 61;
        } else {
            motion->motionAroundPointAngle(enemy_locked_obs->pos_x, enemy_locked_obs->pos_y, atan2(enemy_locked_obs->pos_y - 0, enemy_locked_obs->pos_x - 400) RAD2DEG, atan2(enemy_locked_obs->pos_y - final_pose->x, enemy_locked_obs->pos_x - final_pose->y) RAD2DEG, 100, 160, 100, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        }

        // if (delta_angle(sudut_aku_ke_obs, sudut_teman_ke_obs) < 60 && Pythagoras(enemy_locked_obs.pos_x, enemy_locked_obs.pos_y, robot_data[n_robot_att].pos_x, robot_data[n_robot_att].pos_y) < 200)
        // {
        //     MotionAroundPoint(enemy_locked_obs.pos_x, enemy_locked_obs.pos_y, 300, atan2(enemy_locked_obs.pos_y - 0, enemy_locked_obs.pos_x - 400) RAD2DEG, 130, 90);
        // }
        // else if (delta_angle(sudut_aku_ke_obs, sudut_teman_ke_obs) > 60)
        // {
        //     MotionAroundPoint(enemy_locked_obs.pos_x, enemy_locked_obs.pos_y, 100, atan2(enemy_locked_obs.pos_y - 0, enemy_locked_obs.pos_x - 400) RAD2DEG, 130, 90);
        // }
        // else {
        //     MotionAroundPoint(enemy_locked_obs.pos_x, enemy_locked_obs.pos_y, 100, atan2(enemy_locked_obs.pos_y - 0, enemy_locked_obs.pos_x - 400) RAD2DEG, 130, 90);

        // }

        // MotionAroundPoint(enemy_locked_obs.pos_x, enemy_locked_obs.pos_y, 100, atan2(enemy_locked_obs.pos_y - 0, enemy_locked_obs.pos_x - 400) RAD2DEG, 130, 90);
        break;
    case 61:
        motion->motionAroundPointAngle(buffer_obs[0], buffer_obs[1], atan2(buffer_obs[1] - 0, buffer_obs[0] - 400) RAD2DEG - 180, atan2(buffer_obs[1] - 0, buffer_obs[0] - 400) RAD2DEG - 180, 100, 130, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
        break;
    }
}

void Defender::EnemyLock(float enemy_x, float enemy_y) const { }
void Defender::Penalty() const { }
void Defender::Park() const { }

void Defender::BlockEnemy() const
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
        if ((ball->pos.x < FIELD_X_0 + 100 && final_pose->x < FIELD_X_0 + 170) || (ball->pos.x > FIELD_X_1 - 100 && final_pose->x > FIELD_X_1 - 170)) {
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
                if (ball->pos.x < FIELD_X_0 + 10 || ball->pos.x > FIELD_X_1 - 10) {
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

void Defender::StandBy(int default_r, float x_arching) const
{
    static float radius = 300;
    static float max_r;

    if (default_r < 200) {
        default_r = 200;
    } else if (default_r > 1600) {
        default_r = 340;
    }

    float center_to_ball = atan2f(ball->pos.y - FIELD_Y_0, ball->pos.x - FIELD_X_1_2);

    float target_x = FIELD_X_1_2 + default_r * cosf(center_to_ball);
    float target_y = FIELD_Y_0 + default_r * sinf(center_to_ball);

    LogWithColor("yellow", "STANDDDDDDD BYYY || X ARCHING: %.2f\n", x_arching);

    // printf("X ARCHING %.2f\n", x_arching);

    if (bs_data->style == 'C') {
        *region_by_style = FIELD_Y_1_2;
    } else {
        *region_by_style = FIELD_Y_3_4;
    }

    if (final_pose->y > *region_by_style) {
        motion->motionToPoint(FIELD_X_1_2, FIELD_Y_1_5, 90, 150, 90, NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND);
    }
    // else
    // {
    //     if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 80 && final_pose->y < FIELD_Y_0 + 130) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110))
    //     {
    //         motion->catchingBall(20, 40);
    //     }
    //     else
    //     {
    //         motion->catchingBall(150, 90, NORMAL_OBSTACLE_FRIEND);
    //     }
    // }

    else if (default_r == 350) {
        if (final_pose->y > FIELD_Y_2_3) {
            motion->motionToPoint(target_x, target_y, motion->RobotAngletoBall(), 90, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND); // coba
        } else {
            motion->motionAroundPointAngle(x_arching, FIELD_Y_0, atan2(FIELD_Y_0 - ball->pos.y, x_arching - ball->pos.x) RAD2DEG, motion->RobotAngletoBall() - 180, default_r, 90, 90, NORMAL_OBSTACLE | INVERT);
        }
    } else {
        if (final_pose->y > FIELD_Y_2_3) {
            motion->motionToPoint(target_x, target_y, motion->RobotAngletoBall(), 180, 90, NORMAL_OBSTACLE | NORMAL_OBSTACLE_FRIEND); // coba
        } else {
            motion->motionAroundPointAngle(x_arching, FIELD_Y_0, atan2(FIELD_Y_0 - ball->pos.y, x_arching - ball->pos.x) RAD2DEG, motion->RobotAngletoBall() - 180, default_r, 150, 90, NORMAL_OBSTACLE | INVERT);
        }
        // motion->motionToPoint(target_x, target_y, RobotAngletoBall(), 180, 90, NORMAL_OBSTACLE_BALL);
    }
}
void Defender::Intercept() const
{
    LogWithColor("white", "INTERCEPT : KEJAR BOLA LANGSUNG\n");

    // static float sudut_bola_ke_prev_ball;
    // static float sudut_aku_ke_bola;
    // float x_intercept = ((100 - ball->pos.y) / (*prev_ball[1] - ball->pos.y)) * (*prev_ball[0] - ball->pos.x) + ball->pos.x;

    // if (x_intercept < FIELD_X_0 + 50)
    // {
    //     x_intercept = FIELD_X_0 + 50;
    // }
    // else if (x_intercept > FIELD_X_1 - 50)
    // {
    //     x_intercept = FIELD_X_1 - 50;
    // }

    // // float velocity_ball = Pythagoras(ball->pos.x, ball->pos.y, *prev_ball[0], *prev_ball[1]);
    // // printf("BALL VELOCITI %.2f \n", velocity_ball);

    // if ((ball->distance < 90) &&
    //     final_pose->y < FIELD_Y_1_4 &&
    //     ball->pos.y < FIELD_Y_1_4)
    // {
    //     // CatchBallAdvance(0.6, 200, 1);
    //     motion->catchingBall(190, 100);
    //     // motion->receiveBall3(0, 0, 200, 190);
    // }
    // else
    // {
    //     motion->motionToPoint(x_intercept, 100, motion->RobotAngletoBall(), 250, 90, NORMAL_OBSTACLE);
    // }
    // (ball->pos.y < FIELD_Y_0 + 80 && (final_pose->y < FIELD_Y_0 + 130 && final_pose->x < FIELD_X_1 - 150 && final_pose->x > FIELD_X_0 + 150 )) ||
    if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 80 && final_pose->y < FIELD_Y_0 + 130) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
        motion->catchingBall(20, 40);
    } else {
        motion->catchingBall(200, 90, NORMAL_OBSTACLE_FRIEND);
    }
}

void Defender::preparationGlobal() const
{
    game_sub_status_prep->reentry(0, 0.5);

    // obstacle:

    bs_data->obs_bs_regional[0] = 1;
    bs_data->obs_bs_regional[1] = 7;
    bs_data->obs_bs_regional[2] = 5;
    printf("Defender::preparationGeneral() %d || %d %d || %d %d %d \n", game_sub_status_prep->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, bs_data->obs_bs_regional[0], bs_data->obs_bs_regional[1], bs_data->obs_bs_regional[2]);

    if (ball->is_caught > 0) {
        kickerControl(stm_data, OFFBALLWITHDRIBBLE);
    }

    static double last_time_buzzer_triggered = 0;
    double time_now = ros::Time::now().toSec();

    if (time_now - last_time_buzzer_triggered > 300) {
        buzzerControl(stm_data, 10, 10);
        last_time_buzzer_triggered = time_now;
    }

    if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
        motion->manualMotion(0, 0, 0);
    }

    // motion->manualMotion(0, 0, 0);
    // return;
    //
    // switch (game_sub_status_prep->value)
    // {
    // case 0:
    //     if (pythagoras(final_pose->x, final_pose->y, BASE_2.x, BASE_2.y) > 20)
    //     {
    //         game_sub_status_prep->value = 1;
    //     }
    //     motion->manualMotion(0, 0, 0);
    //     // motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 150, 150);
    //
    //     break;
    // case 1:
    //     if ((final_pose->y - BASE_2.y) > 10)
    //     {
    //         game_sub_status_prep->value = 11;
    //     }
    //     else if (fabs(final_pose->y - BASE_2.y) > 10)
    //     {
    //         game_sub_status_prep->value = 22;
    //     }
    //     else
    //     {
    //         game_sub_status_prep->value = 33;
    //     }
    //
    //     break;
    // case 11:
    //     if (motion->ruckigToPoint(BASE_2.x, BASE_2.y + 10, BASE_2.theta, 250, 200))
    //     {
    //         game_sub_status_prep->value = 0;
    //     }
    //     break;
    // case 22:
    //     if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 250, 200))
    //     {
    //         game_sub_status_prep->value = 0;
    //     }
    //     break;
    // case 33:
    //     if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 220))
    //     {
    //         game_sub_status_prep->value = 0;
    //     }
    //     break;
    // }
}

void Defender::gameStyleA() const
{
    static float pos_x_dapat_bola = 0;
    static float pos_y_dapat_bola = 0;
    static float error_sudut_hadap = 0;

    float pos_x_teman = shared_robot_data[2]->pose.x;
    float pos_y_teman = shared_robot_data[2]->pose.y;

    static float robot2_posisi_x = 0;
    static float robot2_posisi_y = 0;

    static float robot3_posisi_x = 0;
    static float robot3_posisi_y = 0;

    static float robot3_posisi_balik_x = 0;
    static float robot3_posisi_balik_y = 0;

    static float pos_x_buffer = 0;
    static float pos_y_buffer = 0;

    robot2_posisi_x = pos_x_teman;
    robot2_posisi_y = pos_y_teman;

    int8_t obs_kiper = bs_data->obs_bs_regional[0];
    int8_t obs_tengah = bs_data->obs_bs_regional[1];
    int8_t obs_pinggir = bs_data->obs_bs_regional[2];

    // printf("Defender::gameStyleA() %d || %d %d\n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition);
    printf("Defender::gameStyle A %d || %d %d || %d %d\n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, ball->status, ball->is_caught);

    if (pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y) < 100 && pos_x_dapat_bola != 0 && pos_y_dapat_bola != 0) {
        ROS_ERROR("A | BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y));
    }
    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    /*
     *   ============================================================
     *           ROBOT 3 TERIMA BOLA 1//
     *   ============================================================
     */
    case 0:
        stm_data->global_kicker_position = 800;

        printf("PERSIAPAN TERIMA BOLA 1");
        *robot_condition = 0;

        if (motion->ruckigToPoint(40, 570, angleToTarget(400, 600, *final_pose), 250, 200)) {
            game_sub_status->value = 11;
        }

        break;
    case 11:
        printf("TUNGGU TERIMA BOLA\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }
        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 1;
            *robot_condition = 0;
        }
        break;
    case 1:
        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
                game_sub_status->value = 1111; //------------------------------------------------------
                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;
                // buzzerControl(stm_data, 10, 10);
            }
        }

        break;
    /*
     *   ============================================================
     *           ROBOT 3 TENDANG UMPAN BOLA 1    }} [[]]
     *   =================================//==============//=============
     */
    case 1111:
        motion->manualMotion(0, 0, 0);
        robot2_posisi_x = 650;
        robot2_posisi_y = 510;

        if (obs_pinggir == 4 || obs_pinggir == 1) { // //
            robot3_posisi_x = 150;
            robot3_posisi_y = 485;

            robot3_posisi_y = pos_y_dapat_bola - 100;

            if (robot3_posisi_y < 470)
                robot3_posisi_y = 470;
        } else {
            robot3_posisi_x = 200;
            robot3_posisi_y = 480;
        }
        game_sub_status->value = 20;
        break;

    case 20:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
                motion->catchingBall(10, 10);
            } else {
                motion->catchingBall(100, 10);
            }
            printf("CARI BOLA ---------------------------- \n");
            break;
        }
        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(650, 500, *final_pose), 150, 100)) {
            game_sub_status->value = 2;
            // buzzerControl(stm_data, 10, 10);
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200) {
            game_sub_status->value = 2;
        }
        break;

    case 2:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
                motion->catchingBall(10, 10);
            } else {
                motion->catchingBall(100, 10);
            }
            printf("CARI BOLA ---------------------------- \n");
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 150, 180)) {
            game_sub_status->value = 222;
            // *robot_condition = 11;
            // buzzerControl(stm_data, 10, 10);
        }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 100 && fabs(error_sudut_hadap) < 2.1) {
            game_sub_status->value = 222;
            // *robot_condition = 11;
        }
        break;
    case 222:
        game_sub_status->resetUptimeTimeout();
        game_sub_status->value = 10000;
        break;
    case 10000:
        motion->manualMotion(0, 0, 0);
        // *robot_condition = 11;

        game_sub_status->timeout(3, 0.2);
        break;
    case 3:
        printf("TENDANG BOLA 1\n");

        // motion->manualMotion(0, 0, 0);
        motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 0, 45);

        if (ball->is_caught == 0) {
            game_sub_status->value = 31;
            *robot_condition = 11;
            // buzzerControl(stm_data, 10, 10);
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 9.6);
        }
        *robot_condition = 11;
        break;

        /*
         *   ============================================================
         *           ROBOT 3 TERIMA BOLA 2
         *   ============================================================
         */
    case 31:
        motion->manualMotion(0, 0, 0);

        if (obs_pinggir == 4) {
            if (obs_tengah == 7) {
                robot3_posisi_x = 70;
                robot3_posisi_y = 250; // 485

                robot2_posisi_x = 550;
                robot2_posisi_y = 500;
            } else {
                robot3_posisi_x = 70;
                robot3_posisi_y = 340; // 485

                robot2_posisi_x = 550;
                robot2_posisi_y = 500;
            }

            game_sub_status->value = 41; //
            break;
        } else if (obs_pinggir == 1) {
            if (obs_tengah == 7) {
                robot3_posisi_x = 50;
                robot3_posisi_y = 250; // 485

                robot2_posisi_x = 550;
                robot2_posisi_y = 500;
            } else {
                robot3_posisi_x = 50;
                robot3_posisi_y = 360; // 485

                robot2_posisi_x = 550;
                robot2_posisi_y = 500;
            }
        } else if (obs_pinggir == 3 || obs_pinggir == 2) {
            robot3_posisi_x = 40;
            robot3_posisi_y = 360; // 480

            robot2_posisi_x = 550;
            robot2_posisi_y = 500;
        } else // 5 || 6
        {
            robot3_posisi_x = 40;
            robot3_posisi_y = 365; // 480

            robot2_posisi_x = 550;
            robot2_posisi_y = 500;
        }
        game_sub_status->value = 4;

        break; // dsafaaf
    case 41:
        printf("PERSIAPAN TERIMA BOLA 2\n");

        if (motion->ruckigToPoint(30, 400, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 200, 100)) {
            *robot_condition = 0;

            game_sub_status->value = 4;
            // buzzerControl(stm_data, 10, 10);
        }

        if (final_pose->x < 130) {
            game_sub_status->value = 4; //////////
        }

        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 2\n");

        if (motion->motionToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 300, 100)) {
            *robot_condition = 0;
            if (shared_robot_data[2]->robot_condition == 11) {
                game_sub_status->value = 5;
                buzzerControl(stm_data, 10, 10);
            }
        }

        break;
    case 5:
        *robot_condition = 0;
        printf("TERIMA BOLA 2\n");

        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
                game_sub_status->value = 55;
                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;
                // buzzerControl(stm_data, 10, 10);
            }
        }

        break;
    case 55:
        *robot_condition = 0;
        printf("DRIBBLE SEMPURNA\n");

        // motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 2) {
            game_sub_status->value = 551; // -------------------------------------------------------
            // buzzerControl(stm_data, 10, 10);
        } else {
            if ((ball->pos.x < FIELD_X_0 + 10 && final_pose->x < FIELD_X_0 + 110) || (ball->pos.x > FIELD_X_1 - 10 && final_pose->x > FIELD_X_1 - 110) || (ball->pos.y < FIELD_Y_0 + 10 && final_pose->y < FIELD_Y_0 + 110) || (ball->pos.y > FIELD_Y_1 - 10 && final_pose->y > FIELD_X_1 - 110)) {
                motion->catchingBall(10, 10);
            } else {
                motion->catchingBall(100, 90);
            }
            printf("CARI BOLA ---------------------------- \n");
        }

        break;
        /*
         *   ============================================================
         *   ||                ROBOT 3 TENDANG UMPAN 2                 ||
         *   ============================================================
         */
    case 551:
        motion->manualMotion(0, 0, 0);

        robot3_posisi_balik_x = robot3_posisi_x;
        robot3_posisi_balik_y = robot2_posisi_y;

        if (obs_pinggir == 5) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 120;
                robot3_posisi_y = 200; // 365

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 100;
                robot3_posisi_y = 230; // 365

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 6) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 130;
                robot3_posisi_y = 280; // 365

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 160;
                robot3_posisi_y = 270; // 365

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 2) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 200;
                robot3_posisi_y = 230; // 360

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 220;
                robot3_posisi_y = 270; // 360

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 3) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 120;
                robot3_posisi_y = 120; // 360

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 200;
                robot3_posisi_y = 250; // 360

                robot2_posisi_x = 700;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 1) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 120;
                robot3_posisi_y = 140; // 250

                robot2_posisi_x = 700;
                robot2_posisi_y = 237;

                robot3_posisi_y = pos_y_dapat_bola - 101;

                if (robot3_posisi_y < 100) {
                    robot3_posisi_y = 100;
                }
            } else {
                robot3_posisi_x = 220;
                robot3_posisi_y = 260; // 360

                robot2_posisi_x = 700;
                robot2_posisi_y = 237;
            }
        } else if (obs_pinggir == 4) //* =============================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 100;
                robot3_posisi_y = 120; // 250

                robot2_posisi_x = 700;
                robot2_posisi_y = 180;

                robot3_posisi_y = pos_y_dapat_bola - 100;

                if (robot3_posisi_y < 100) {
                    robot3_posisi_y = 100;
                }
            } else if (obs_tengah == 8) {
                robot3_posisi_x = 220;
                robot3_posisi_y = 260; // 340

                robot2_posisi_x = 700;
                robot2_posisi_y = 300;
            }
        }

        game_sub_status->value = 6;

        break;

    case 56:
        printf("PERSIAPAN TENDANG BOLA 2\n"); //* GIRING BOLA
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 10);
            printf("CARI BOLA ---------------------------- \n");
            break;
        }
        if (motion->ruckigToPoint(robot3_posisi_balik_x, robot3_posisi_balik_y, angleToTarget(robot2_posisi_x, robot2_posisi_y - 10, *final_pose), 160, 100)) {
            game_sub_status->value = 6;
            // buzzerControl(stm_data, 10, 10);
        }
        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200) {
            game_sub_status->value = 6;
        }
        break;
    case 6:
        printf("PERSIAPAN TENDANG 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y - 10, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y - 10, *final_pose), 75, 100)) {
            game_sub_status->value = 69;

            // buzzerControl(stm_data, 10, 10);
        }

        // if (obs_pinggir == 4 || obs_pinggir == 1)
        // {
        //     if (pythagoras(robot3_posisi_x, robot3_posisi_y, final_pose->x, final_pose->y) < 80 && fabs(error_sudut_hadap) < 1.4)
        //     {
        //         game_sub_status->value = 69;
        //     }
        // }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200 && fabs(error_sudut_hadap) < 1.4 && final_pose->y < robot3_posisi_y + 30) {
            game_sub_status->value = 69;
        }

        printf("pos: %.2f %.2f || %.2f %.2f \n", robot3_posisi_x, robot3_posisi_y, pos_x_dapat_bola, pos_y_dapat_bola);
        break;
    case 69:
        game_sub_status->resetUptimeTimeout();
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 10001;
        break;
    case 10001:
        motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y - 5, *final_pose), 0, 45);

        // *robot_condition = 11;
        if ((obs_pinggir == 5 && obs_tengah == 8) || (obs_pinggir == 6 && obs_tengah == 7)) {
            game_sub_status->timeout(7, 0.65);
        } else if ((obs_pinggir == 2 && obs_tengah == 8) || (obs_pinggir == 3 && obs_tengah == 7)) {
            game_sub_status->timeout(7, 0.65);
        } else {
            game_sub_status->timeout(7, 0.1);
        }

        break;
    case 7:
        printf("TENDANG BOLA 2\n");

        motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y - 5, *final_pose), 0, 45);

        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
            // buzzerControl(stm_data, 10, 10);
        } else {

            if (final_pose->x > 125) {
                kickerControl(stm_data, THROTTLE_AIM, 9);
            } else {
                if (obs_pinggir == 3 && obs_tengah == 7) {
                    kickerControl(stm_data, THROTTLE_AIM, 9.5);
                } else {
                    kickerControl(stm_data, THROTTLE_AIM, 10.4);
                }
            }
        }
        *robot_condition = 11;
        break;
    case 8:
        // *robot_condition = 11;

        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 999;

        break;

    case 99:
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 999;
        break;

    case 999:
        // game_status->value = 'K';
        // if (pythagoras(BASE_2.x, BASE_2.y, final_pose->x, final_pose->y) < 50)
        // {
        if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
            motion->manualMotion(0, 0, 0);
        }
        // }
        // preparationGlobal();
        break;
    }
}

void Defender::gameStyleB() const
{
    static float pos_x_dapat_bola = 0;
    static float pos_y_dapat_bola = 0;
    static float error_sudut_hadap = 0;

    static float arah_goal_x = 300;

    static float robot2_posisi_x = 0;
    static float robot2_posisi_y = 0;

    static float robot3_posisi_x = 0;
    static float robot3_posisi_y = 0;

    static float pos_x_buffer = 0;
    static float pos_y_buffer = 0;

    float pos_x_teman = shared_robot_data[2]->pose.x;
    float pos_y_teman = shared_robot_data[2]->pose.y;

    robot2_posisi_x = pos_x_teman;
    robot2_posisi_y = pos_y_teman;

    static float robot3_posisi_balik_x = 0;
    static float robot3_posisi_balik_y = 0;

    int8_t obs_kiper = bs_data->obs_bs_regional[0];
    int8_t obs_tengah = bs_data->obs_bs_regional[1];
    int8_t obs_pinggir = bs_data->obs_bs_regional[2];

    float sudut_target = 0;

    printf("Defender::gameStyleB() %d || %d %d || %d \n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, ball->is_caught);

    if (pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y) < 100 && pos_x_dapat_bola != 0 && pos_y_dapat_bola != 0) {
        ROS_ERROR("B | BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y));
    }

    game_sub_status->reentry(0, 0.5);
    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }
    switch (game_sub_status->value) {
        /*
         *   ============================================================
         *           ROBOT 3 AMBIL BOLA
         *   ============================================================
         */
    // case 0:
    //     stm_data->global_kicker_position = 800;

    //     printf("AMBIL BOLA\n");
    //     if (motion->ruckigToPoint(150, 540, -10, 250, 200))
    //     {
    //         *robot_condition = 0;
    //         game_sub_status->value = 101;
    //         // buzzerControl(stm_data, 10, 10);
    //     }

    //     if (pythagoras(final_pose->x, final_pose->y, 150, 540) < 100)
    //     {

    //         game_sub_status->value = 101;
    //         *robot_condition = 0;
    //         // buzzerControl(stm_data, 10, 10);
    //     }
    //     break;
    case 0:
        printf("AMBIL BOLA\n");

        // game_sub_status->value = 5;
        if (motion->ruckigToPoint(322, 640, -2, 250, 200)) {
            *robot_condition = 0;
            game_sub_status->value = 1;
            // buzzerControl(stm_data, 10, 10);
        }

        // if (pythagoras(final_pose->x, final_pose->y, 320, 640) < 55)
        // {
        //     game_sub_status->value = 1;
        //     *robot_condition = 0;
        //     // buzzerControl(stm_data, 10, 10);
        // }
        break;
    case 1:
        printf("CATCHING BALL\n");
        if (motion->catchingBall(65, 50)) {
            game_sub_status->value = 2;
        }
        break;
    case 2:
        // if (motion->pivotMotion(angleToTarget(680, 495, *final_pose), 90))

        if (motion->pivotMotion(angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y - 3, *final_pose), 90)) {
            game_sub_status->value = 3;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y - 3, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (fabs(error_sudut_hadap) < 5.5) {
            game_sub_status->value = 3;
        }

        break;
    case 3:
        printf("TENDANG BOLA 1 || UMPAN BOLA 1 \n");
        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 2) {
            kickerControl(stm_data, NO_AIM, 8.5);
            game_sub_status->value = 31;
            // buzzerControl(stm_data, 10, 10);
        }
        break;
        /*
         *   ============================================================
         *           ROBOT 3 TERIMA BOLA 1
         *   ============================================================
         */
    case 31:
        robot3_posisi_x = 110;
        robot3_posisi_y = 500;

        robot2_posisi_x = 550;
        robot2_posisi_y = 500;

        game_sub_status->value = 4;

        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 1\n");
        *robot_condition = 11;

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 270, 100)) {
            *robot_condition = 0;
            game_sub_status->value = 44;
            // buzzerControl(stm_data, 10, 10);
        }

        // if (shared_robot_data[2]->robot_condition == 11)
        // {
        //     game_sub_status->value = 4;
        //     buzzerControl(stm_data, 10, 10);
        // }
        break;
    case 44:
        printf("TUNGGU TERIMA BOLA 1\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }

        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 5;
            *robot_condition = 0;

            buzzerControl(stm_data, 10, 10);
        }
        break;
    case 5:
        printf("TERIMA BOLA 1\n");

        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
                *robot_condition = 0;
                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;

                if (final_pose->x > 120 && obs_pinggir == 4) {
                    game_sub_status->value = 52;
                } else {
                    game_sub_status->value = 53;
                }
                // buzzerControl(stm_data, 10, 10);
            }
        }
        break;
    case 52:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(30, 400, 45, 150, 100)) {
            game_sub_status->value = 53;
            // buzzerControl(stm_data, 10, 10);
        }

        // if (motion->ruckigToPoint(30, 400, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 150, 100))
        // {
        //     game_sub_status->value = 53;
        //     // buzzerControl(stm_data, 10, 10);
        // }

        // if (motion->ruckigToPoint(30, 400, -90, 150, 100))
        // {
        //     game_sub_status->value = 53;
        //     // buzzerControl(stm_data, 10, 10);
        // }

        if (pythagoras(30, 400, final_pose->x, final_pose->y) < 70) {
            game_sub_status->value = 53;
        }

        if (final_pose->x < 130) {
            game_sub_status->value = 53;
        }

        break;

        /*
         *   ============================================================
         *           ROBOT 3 TENDANG UMPAN 2 || 4
         *   ============================================================
         */
    case 53:
        motion->manualMotion(0, 0, 0);

        robot3_posisi_balik_x = robot3_posisi_x;
        robot3_posisi_balik_y = robot3_posisi_y;

        if (1 == obs_pinggir) {
            if (obs_tengah == 7) {
                robot3_posisi_x = 30;
                robot3_posisi_y = 250;

                robot2_posisi_x = 700;
                robot2_posisi_y = 250;
            } else {
                robot3_posisi_x = 30;
                robot3_posisi_y = 320;

                robot2_posisi_x = 700;
                robot2_posisi_y = 300;
            }
        } else if (4 == obs_pinggir) {
            if (obs_tengah == 7) {
                robot3_posisi_x = 30;
                robot3_posisi_y = 250;

                robot2_posisi_x = 680;
                robot2_posisi_y = 450;
            } else {
                robot3_posisi_x = 30;
                robot3_posisi_y = 320;

                robot2_posisi_x = 680;
                robot2_posisi_y = 450;
            }
        } else if (obs_pinggir == 6) {

            if (obs_tengah == 7) {
                robot3_posisi_x = 30;
                robot3_posisi_y = 370;

                robot2_posisi_x = 680;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 80;
                robot3_posisi_y = 370;

                robot2_posisi_x = 680;
                robot2_posisi_y = 400;
            }
        } else {
            robot3_posisi_x = 30;
            robot3_posisi_y = 370;

            robot2_posisi_x = 680;
            robot2_posisi_y = 400;
        }

        if (obs_pinggir == 5 || obs_pinggir == 2 || obs_pinggir == 6 || obs_pinggir == 3) {
            game_sub_status->value = 61;
        } else {
            game_sub_status->value = 6;
        }
        break;
    case 6:
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = 45 - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, 45, 150, 100)) // 150, 100
        {
            game_sub_status->value = 666;
            // buzzerControl(stm_data, 10, 10);
        }

        if (pythagoras(final_pose->x, final_pose->y, robot3_posisi_x, robot3_posisi_y) < 70) {
            game_sub_status->value = 666;
        }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200 && fabs(error_sudut_hadap) < 1.4 && final_pose->y < robot3_posisi_y + 10) {
            game_sub_status->value = 666;
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 61:
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = 5 - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, 5, 150, 100)) // 150, 100
        {
            game_sub_status->value = 666;
            // buzzerControl(stm_data, 10, 10);
        }

        // if (pythagoras(final_pose->x, final_pose->y, robot3_posisi_x, robot3_posisi_y) < 70)
        // {
        //     game_sub_status->value = 666;
        // }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200 && fabs(error_sudut_hadap) < 1.4 && final_pose->y < robot3_posisi_y) {
            game_sub_status->value = 666;
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 666:
        // motion->manualMotion(0, 0, 0);

        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y + 5, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if ((motion->motionToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y + 5, *final_pose), 0, 150)) && (fabs(error_sudut_hadap) < 1)) {
            // game_sub_status->value = 7;
            game_sub_status->timeout(7, 0.4);
        }

        if (fabs(error_sudut_hadap) < 1) {
            // game_sub_status->value = 7;

            game_sub_status->timeout(7, 0.4);
        }

        break;
    case 7:
        printf("TENDANG BOLA 2\n");
        // motion->manualMotion(0, 0, 0);

        motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y + 20, *final_pose), 0, 45);

        if (ball->is_caught == 0) //
        {
            game_sub_status->value = 71;
            // buzzerControl(stm_data, 10, 10);
            *robot_condition = 11;
        } else {
            if (obs_pinggir == 2 || obs_pinggir == 5 || obs_pinggir == 3 || obs_pinggir == 6) {
                kickerControl(stm_data, THROTTLE_AIM, 12); //
                *robot_condition = 11;
            } else {
                kickerControl(stm_data, THROTTLE_AIM, 12);
                *robot_condition = 11;
            }
        }

        pos_x_buffer = final_pose->x;
        pos_y_buffer = final_pose->y;
        break;
        /*
         *   ============================================================
         *           ROBOT 3 TERIMA BOLA 2 || 5
         *   ============================================================
         */
    case 71:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_balik_x = robot3_posisi_x;
        robot3_posisi_balik_y = robot3_posisi_y;

        if (obs_pinggir == 1) //*==========  1  ===========================
        {
            if (7 == obs_tengah) {
                robot3_posisi_x = 100;
                robot3_posisi_y = 150; // 230

                robot2_posisi_x = 700;
                robot2_posisi_y = 270;
            } else if (8 == obs_tengah) {
                robot3_posisi_x = 150;
                robot3_posisi_y = 230; // 300

                robot3_posisi_y = pos_y_buffer - 100;

                if (robot3_posisi_y < 230) {
                    robot3_posisi_y = 230;
                }

                robot2_posisi_x = 700;
                robot2_posisi_y = 370;
            }
        } else if (obs_pinggir == 2) //*==========  2  ===========================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 180;
                robot3_posisi_y = 230; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 170;
                robot3_posisi_y = 279; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 3) //*============  3  =========================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 70;
                robot3_posisi_y = 100; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 180;
                robot3_posisi_y = 250; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;

                robot3_posisi_y = pos_y_buffer - 100;

                if (robot3_posisi_y < 230) {
                    robot3_posisi_y = 230;
                }
            }
        } else if (obs_pinggir == 4) //*==============  4  =======================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 100;
                robot3_posisi_y = 150; // 250

                robot2_posisi_x = 650;
                robot2_posisi_y = 450;

                robot3_posisi_y = pos_y_buffer - 100;

                if (robot3_posisi_y < 120) {
                    robot3_posisi_y = 120;
                }
            } else {
                robot3_posisi_x = 150;
                robot3_posisi_y = 240; // 300

                robot2_posisi_x = 650;
                robot2_posisi_y = 450;
            }
        } else if (obs_pinggir == 5) //*==============  5  =======================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 50;
                robot3_posisi_y = 210; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 100;
                robot3_posisi_y = 180; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            }
        } else if (obs_pinggir == 6) //*=============  6  ========================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 170;
                robot3_posisi_y = 310; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 210;
                robot3_posisi_y = 270; // 370

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            }
        } else //*=============================================================
        {
            if (obs_tengah == 7) {
                robot3_posisi_x = 200;
                robot3_posisi_y = 360;

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            } else {
                robot3_posisi_x = 200;
                robot3_posisi_y = 360;

                robot2_posisi_x = 750;
                robot2_posisi_y = 400;
            }
        }

        game_sub_status->value = 8;

        break;
    case 77:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->ruckigToPoint(50, final_pose->y - 50, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 200, 100)) {
            game_sub_status->value = 8;
            // buzzerControl(stm_data, 10, 10);
        }

        if (final_pose->x < 50) {
            game_sub_status->value = 8;
        }
        break;
    case 8:
        printf("PERSIAPAN TERIMA BOLA 2\n");
        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 250, 100)) {
            game_sub_status->value = 9;
            // buzzerControl(stm_data, 10, 10);
        }
        break;
    case 9:
        printf("TUNGGU TERIMA BOLA 2\n");
        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }

        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 91;
            *robot_condition = 0;

            buzzerControl(stm_data, 10, 10);
        }
        break;
    case 91:
        printf("TERIMA BOLA 2\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
                game_sub_status->value = 92;
                *robot_condition = 0;
                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;

                // buzzerControl(stm_data, 10, 10);
            }
        }
        break;
    case 92:
        printf("DRIBBLE SEMPURNA\n");

        if (ball->is_caught == 2) {
            game_sub_status->value = 93;
            // buzzerControl(stm_data, 10, 10);
        }

        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }
        break;

        /*
         *   ============================================================
         *           ROBOT 3 TENDANG GOAL
         *   ============================================================
         */
    case 93:

        robot3_posisi_balik_x = robot3_posisi_x;
        robot3_posisi_balik_y = robot3_posisi_y;

        if (obs_kiper == 3) {
            if (obs_pinggir == 6) //* ================  6  =======================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 230;
                    robot3_posisi_y = 360;

                    arah_goal_x = 420;

                    game_sub_status->value = 94;
                    break;
                } else {
                    robot3_posisi_x = 250;
                    robot3_posisi_y = 360;

                    arah_goal_x = 430;

                    game_sub_status->value = 96;
                    break;
                }
            } else if (obs_pinggir == 5) //* =================  5  ======================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 190;
                    robot3_posisi_y = 230;

                    arah_goal_x = 430;
                } else {
                    robot3_posisi_x = 150;
                    robot3_posisi_y = 200;

                    arah_goal_x = 430;
                }
            } else if (obs_pinggir == 3) //* =================  3  ======================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 200;
                    robot3_posisi_y = 250;

                    arah_goal_x = 425;
                } else {
                    robot3_posisi_x = 150;
                    robot3_posisi_y = 250;

                    arah_goal_x = 430;

                    game_sub_status->value = 96;
                    break;
                }
            } else if (obs_pinggir == 2) //* ===============  2  ========================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 150;
                    robot3_posisi_y = 250;

                    arah_goal_x = 450;

                    game_sub_status->value = 96;
                    break;
                } else {
                    robot3_posisi_x = 150;
                    robot3_posisi_y = 250;

                    arah_goal_x = 430;

                    game_sub_status->value = 96;
                    break;
                }
            } else if (obs_pinggir == 4) //* ==============  4  =========================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 100;
                    robot3_posisi_y = 230;

                    arah_goal_x = 410;

                    game_sub_status->value = 94;
                    break;
                } else {
                    robot3_posisi_x = 250;
                    robot3_posisi_y = 230;

                    arah_goal_x = 420;

                    game_sub_status->value = 96;
                    break;
                }
            } else if (obs_pinggir == 1) //* ====================  1  ==========================
            {
                if (obs_tengah == 7) {
                    robot3_posisi_x = 100;
                    robot3_posisi_y = 250;

                    arah_goal_x = 430;

                    game_sub_status->value = 94;
                    break;
                } else {
                    robot3_posisi_x = 180;
                    robot3_posisi_y = 250;

                    arah_goal_x = 430;
                    game_sub_status->value = 94;
                    break;
                }
            } else {
                robot3_posisi_x = 180;
                robot3_posisi_y = 250;

                arah_goal_x = 430;
            }

            game_sub_status->value = 94;
            break;
        } else //* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        {
            //* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            if (obs_pinggir == 6) //* ====================  6  ==========================
            {
                robot3_posisi_x = 230;
                robot3_posisi_y = 330;

                arah_goal_x = 320;

                if (obs_tengah == 7) {
                    game_sub_status->value = 94;
                } else {
                    game_sub_status->value = 94;
                }
                break;
            } else if (obs_pinggir == 1) //* ====================  1  ==========================
            {
                robot3_posisi_x = 250;
                robot3_posisi_y -= 5;

                arah_goal_x = 335;
                game_sub_status->value = 96;
                break;
            } else if (obs_pinggir == 4 || obs_pinggir == 2 || obs_pinggir == 3) //* ====================  4 | 2 | 3  ==========================
            {
                arah_goal_x = 320;
                game_sub_status->value = 96;
                break;
            } else //* ====================  5  ==========================
            {
                // obs_pinggir == 5
                if (pos_x_dapat_bola > 170) {
                    robot3_posisi_x = 250;
                    robot3_posisi_y = 360;

                    arah_goal_x = 330;
                } else {
                    robot3_posisi_x = 50;
                    robot3_posisi_y = 360;

                    arah_goal_x = 350;
                }

                game_sub_status->value = 96;
                break;
            }
            break;
        }
        break;
    case 94:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->motionToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(arah_goal_x, 0, *final_pose), 150, 100)) {
            game_sub_status->value = 102;
        }

        if (pythagoras(final_pose->x, final_pose->y, robot3_posisi_x, robot3_posisi_y) < 120 && fabs(error_sudut_hadap) < 3) {
            game_sub_status->value = 102;
        }

        break;

    case 96:

        if (final_pose->y < 180 && final_pose->x > 200) {
            error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

            if (error_sudut_hadap < -180) {
                error_sudut_hadap += 360;
            } else if (error_sudut_hadap > 180) {
                error_sudut_hadap -= 360;
            }

            printf("PERSIAPAN TENDANG GOAL\n");

            if (motion->motionToPoint(200, 150, angleToTarget(arah_goal_x, 0, *final_pose), 150, 150)) {
                game_sub_status->value = 102;
            }

            if (fabs(error_sudut_hadap) < 3) {
                game_sub_status->value = 102;
            }
        } else {
            game_sub_status->value = 97;
        }

        break;
    case 97:
        error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        printf("PERSIAPAN TENDANG GOAL\n");
        if (motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(arah_goal_x, 0, *final_pose), 150, 150)) {
            game_sub_status->value = 102;
        }

        if (fabs(error_sudut_hadap) < 3) {
            game_sub_status->value = 102;
        }

        break;
    case 10:

        error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        printf("PERSIAPAN TENDANG GOAL\n");
        if (motion->motionToPoint(robot3_posisi_x + 10, robot3_posisi_y - 70, angleToTarget(arah_goal_x, 0, *final_pose), 100, 70)) {
            game_sub_status->value = 102;
        }

        if (pythagoras(final_pose->x, final_pose->y, robot3_posisi_x + 10, robot3_posisi_y - 70) < 10 && fabs(error_sudut_hadap) < 2.5) {
            game_sub_status->value = 102;
        }

        break;
    case 102:
        motion->manualMotion(0, 0, 0);

        game_sub_status->value = 11;
        break;

    case 103:
        motion->manualMotion(0, 0, 0);
        game_sub_status->timeout(11, 0.1);
        break;

    case 11:
        printf("TENDANG GOAL\n");
        motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(arah_goal_x, 0, *final_pose), 0, 45);

        if (ball->is_caught == 0) {
            game_sub_status->value = 12;
            // buzzerControl(stm_data, 10, 10);
        } else {
            kickerControl(stm_data, NO_AIM, 21);
        }
        break;
    case 12:
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 99;
        break;
    case 99:
        // game_status->value = 'K';
        // motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 100);
        if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}

void Defender::gameStyleC() const
{
    static float pos_x_dapat_bola = 0;
    static float pos_y_dapat_bola = 0;
    static float error_sudut_hadap = 0;

    static float arah_goal_x = 0;

    float pos_x_teman = shared_robot_data[2]->pose.x;
    float pos_y_teman = shared_robot_data[2]->pose.y;

    static float robot2_posisi_x = 0;
    static float robot2_posisi_y = 0;

    static float robot3_posisi_x = 0;
    static float robot3_posisi_y = 0;

    robot2_posisi_x = pos_x_teman;
    robot2_posisi_y = pos_y_teman - 1;

    int8_t obs_kiper = bs_data->obs_bs_regional[0];
    int8_t obs_tengah = bs_data->obs_bs_regional[1];
    int8_t obs_pinggir = bs_data->obs_bs_regional[2];

    // printf("Defender::gameStyleC() %d\n", game_sub_status->value);
    printf("Defender::gameStyleC() %d || %d %d || %d %d\n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, ball->status, ball->is_caught);
    if (pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y) < 100 && pos_x_dapat_bola != 0 && pos_y_dapat_bola != 0) {
        ROS_ERROR("C | BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y));
    }
    game_sub_status->reentry(0, 0.5);
    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    case 0:
        motion->manualMotion(0, 0, 0);

        robot3_posisi_x = 30;
        robot3_posisi_y = 120;

        game_sub_status->value = 1;

        break;
    case 1:
        printf("PERSIAPAN TERIMA BOLA 1");
        stm_data->global_kicker_position = 800;

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(782, 18, *final_pose), 200, 100)) {
            *robot_condition = 0;
            game_sub_status->value = 10;
        }
        break;
    case 10:
        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {

                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;
                game_sub_status->value = 11;
            }
        }
        break;
    case 11:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 150;
        robot3_posisi_y = 100;

        game_sub_status->value = 2;
        break;
    case 2:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(700, 100, *final_pose), 190, 90)) {
            game_sub_status->value = 3;
        }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 100) //* ANJAY NGEBUT
        {
            game_sub_status->value = 3;
        }
        break;

    case 3:
        printf("PERSIAPAN TENDANG BOLA 1\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 190, 120)) {
            game_sub_status->value = 4;
        }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 200 && fabs(error_sudut_hadap) < 1.4 && final_pose->y < 120) {
            game_sub_status->value = 4;
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 4:
        *robot_condition = 11;
        motion->manualMotion(0, 0, 0);

        game_sub_status->timeout(41, 0.4); //
        break;
    case 41:
        printf("TENDANG BOLA 1\n");
        // motion->manualMotion(0, 0, 0);
        motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 0, 45);

        if (ball->is_caught == 0) {
            *robot_condition = 11;
            // game_sub_status->value = 5;
            motion->manualMotion(0, 0, 0);
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 12);
        }

        if (ball->is_caught == 0) {
            game_sub_status->value = 42;
        }
        break;
    case 42:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 30;
        robot3_posisi_y = 100;

        game_sub_status->value = 5;
        break;
        break;
    case 5:
        printf("PERSIAPAN TERIMA BOLA 2\n");

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(700, 120, *final_pose), 150, 120)) {
            game_sub_status->value = 6;
        }
        break;

    case 6:
        printf("TUNGGU TERIMA BOLA 2\n");

        motion->manualMotion(0, 0, 0);
        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 7;
            buzzerControl(stm_data, 10, 10);
        }
        break;

    case 7:
        printf("TERIMA BOLA 2\n");

        *robot_condition = 0;
        if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
            game_sub_status->value = 700;
            pos_x_dapat_bola = final_pose->x;
            pos_y_dapat_bola = final_pose->y;
        }
        break;

    case 700:
        if (obs_pinggir == 6 && obs_kiper == 3) {
            if (final_pose->x > 170) {
                game_sub_status->value = 7000;
            } else {
                game_sub_status->value = 71;
            }
        } else {
            game_sub_status->value = 71;
        }

        break;
    case 7000:
        if (motion->motionToPoint(20, 200, angleToTarget(arah_goal_x, 0, *final_pose), 150, 100)) {
            game_sub_status->value = 71;
        }

        if (motion->RobotDist2Point(20, 200) < 100) {
            game_sub_status->value = 71;
        }
        break;

        /*
         *====================================
         */

    case 71:
        printf("MENENTUKAN ARAH TENDANGAN\n");
        if (obs_kiper == 3 && obs_pinggir == 6) {
            // game_sub_status->value = 91; //* UMPAN TAMBAHAN
            robot3_posisi_x = 170;
            robot3_posisi_y = 380;

            arah_goal_x = 430;
            game_sub_status->value = 72;
        } else if (obs_kiper == 3) {
            robot3_posisi_x = 150;
            robot3_posisi_y = 220;

            arah_goal_x = 430;
            game_sub_status->value = 72; //* GESER KEBELAKANG
        } else if (obs_kiper == 1) {
            robot3_posisi_x = 100;
            robot3_posisi_y = 170;

            arah_goal_x = 350;
            game_sub_status->value = 73;
        } else {
            robot3_posisi_x = 100;
            robot3_posisi_y = 200;

            arah_goal_x = 330;
            game_sub_status->value = 73;
        }
        break;
    case 72:
        printf("PERSIAPAN TEMBAK GOAL"); //* GESER KEBELAKANG
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(arah_goal_x, 0, *final_pose), 100, 90)) {
            game_sub_status->value = 81;
        }

        // if (pythagoras(final_pose->x, final_pose->y, robot3_posisi_x, robot3_posisi_y) < 50)
        // {
        //     game_sub_status->value = 81;
        // }

        break;
    case 73:
        printf("PERSIAPAN TEMBAK GOAL");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(arah_goal_x, 0, *final_pose), 100, 90)) {
            motion->manualMotion(0, 0, 0);
            game_sub_status->value = 81;
        }

        if (final_pose->x < 180 && final_pose->y > 150) {
            motion->manualMotion(0, 0, 0);
            game_sub_status->value = 81;
        }
        break;
    case 81:
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(arah_goal_x, 0, *final_pose), 100, 120)) {
            game_sub_status->value = 9;
        }

        if (obs_kiper == 1) {
            if (fabs(error_sudut_hadap) < 2) {
                motion->manualMotion(0, 0, 0);
                game_sub_status->value = 9;
            }
        } else {
            if (fabs(error_sudut_hadap) < 4) {
                motion->manualMotion(0, 0, 0);
                game_sub_status->value = 9;
            }
        }
        break;
        /*
         *====================================
         *              GOAL
         *====================================
         */

    case 9:
        printf("TENDANG GOAL\n");

        motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(arah_goal_x, 0, *final_pose), 100, 45);
        if (ball->is_caught == 0) {
            game_sub_status->value = 94;
            // buzzerControl(stm_data, 10, 10);
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 18);
        }

        break;
        /*
         *====================================
         *           UMPAN
         *====================================
         */
    case 91:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 150;
        robot3_posisi_y = 100;
        game_sub_status->value = 911;

        break;

    case 911:
        printf("PERSIAPAN TENDANG UMPAN 3\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(700, 100, *final_pose), 190, 120)) {
            game_sub_status->value = 92;
            // game_sub_status->resetUptimeTimeout();
        }

        // if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 50)
        // {
        //     game_sub_status->value = 92;
        // }
        break;
    case 92:
        printf("PERSIAPAN TENDANG UMPAN 3\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 190, 120)) {
            game_sub_status->value = 93;
        }

        // if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 50 && fabs(error_sudut_hadap) < 1.4 && final_pose->y < 130)
        // {
        //     game_sub_status->value = 93;
        // }
        break;
    case 93:
        game_sub_status->resetUptimeTimeout();
        game_sub_status->value = 10001;
        break;
    case 10001:
        motion->manualMotion(0, 0, 0);
        *robot_condition = 11;

        game_sub_status->timeout(100, 0.1);
        break;
    case 100:
        printf("TENDANG BOLA 2\n");
        // motion->manualMotion(0, 0, 0);
        motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 0, 45);

        if (ball->is_caught == 0) {
            game_sub_status->value = 94;
            *robot_condition = 11;
            // buzzerControl(stm_data, 10, 10);
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 11);
        }
        break;
    case 94:
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 99;
        break;
    case 99:
        // motion->manualMotion(0, 0, 0);
        // game_status->value = 'K';
        // motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 100);
        if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
            motion->manualMotion(0, 0, 0);
        }

        break;
    }
}

void Defender::gameStyleD() const
{
    printf("Defender::gameStyle D() %d || %d %d || %d %d || %.2f %.2f || %.2f\n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, ball->status, ball->is_caught, ball->pos.x, ball->pos.y, final_pose->theta);

    static float pos_x_dapat_bola = 0;
    static float pos_y_dapat_bola = 0;
    static float error_sudut_hadap = 0;

    float pos_x_teman = shared_robot_data[2]->pose.x;
    float pos_y_teman = shared_robot_data[2]->pose.y;

    static float robot2_posisi_x = 0;
    static float robot2_posisi_y = 0;

    static float robot3_posisi_x = 0;
    static float robot3_posisi_y = 0;

    robot2_posisi_x = pos_x_teman;
    robot2_posisi_y = pos_y_teman - 1;

    int8_t obs_kiper = bs_data->obs_bs_regional[0];
    int8_t obs_tengah = bs_data->obs_bs_regional[1];
    int8_t obs_pinggir = bs_data->obs_bs_regional[2];

    if (pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y) < 100 && pos_x_dapat_bola != 0 && pos_y_dapat_bola != 0) {
        ROS_ERROR("D | BELUM 50 CM DI CASE %d JARAK : %.2f", game_sub_status->value, pythagoras(pos_x_dapat_bola, pos_y_dapat_bola, robot3_posisi_x, robot3_posisi_y));
    }

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
    // case 0:
    //     stm_data->global_kicker_position = 800;

    //     printf("PERSIAPAN AMBIL BOLA\n");
    //     if (motion->ruckigToPoint(-50, 100, 0, 250, 200))
    //     {
    //         *robot_condition = 0;
    //         game_sub_status->value = 101;
    //         // buzzerControl(stm_data, 10, 10);
    //     }

    //     if (pythagoras(final_pose->x, final_pose->y, -50, 100) < 120)
    //     {
    //         game_sub_status->value = 101;
    //     }

    //     break;
    case 0:
        printf("PERSIAPAN AMBIL BOLA\n");
        if (motion->ruckigToPoint(-90, 30, 1, 150, 100)) {
            *robot_condition = 0;
            game_sub_status->value = 1;
            // buzzerControl(stm_data, 10, 10);
        }

        if (pythagoras(final_pose->x, final_pose->y, -80, 30) < 45) {
            game_sub_status->value = 1;
        }
        break;
    case 1:
        printf("CATCHING BALL\n");
        if (motion->catchingBall(55, 10)) {
            game_sub_status->value = 30;
        }
        break;

    case 30:
        if (motion->pivotMotion(angleToTarget(700, 100, *final_pose), 90)) {
            game_sub_status->value = 3;
        }
        break;
    case 3:
        printf("TENDANG BOLA 1\n");
        motion->pivotMotion(angleToTarget(700, 100, *final_pose), 30);
        // motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 2) {
            kickerControl(stm_data, NO_AIM, 11.5);
            game_sub_status->value = 39; //* =============================
            // game_sub_status->value = 31;
            *robot_condition = 11;
            // buzzerControl(stm_data, 10, 10);
        }
        break;
    case 31:
        motion->manualMotion(0, 0, 0);
        break;
    case 39:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 50;
        robot3_posisi_y = 110;

        robot2_posisi_x = 700;
        robot2_posisi_y = 100;

        game_sub_status->value = 4;

        break;
    case 4:
        printf("PERSIAPAN TERIMA BOLA 1\n");
        *robot_condition = 11;
        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(700, 100, *final_pose), 170, 100)) {
            game_sub_status->value = 44;
            // buzzerControl(stm_data, 10, 10);
        }
        break;
    case 44:
        printf("TUNGGU TERIMA BOLA 1\n");

        if (ball->status > 0) {
            motion->motionToPoint(final_pose->x, final_pose->y, angleToTarget(ball->pos.x, ball->pos.y, *final_pose), 0, 90);
        } else {
            motion->manualMotion(0, 0, 0);
        }

        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 5;
            buzzerControl(stm_data, 10, 10);
        }
        break;
    case 5:
        *robot_condition = 0;

        printf("TERIMA BOLA 1\n");
        if (ball->status == 0 && ball->is_caught == 0) {
            motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, *final_pose), 0, 90);
        } else {
            if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
                game_sub_status->value = 51;
                *robot_condition = 0;
                pos_x_dapat_bola = final_pose->x;
                pos_y_dapat_bola = final_pose->y;
                // buzzerControl(stm_data, 10, 10);
            }
        }
        break;
    case 51:
        printf("DRIBBLE SEMPURNA\n");
        *robot_condition = 0;

        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 2) {
            game_sub_status->value = 56;
            // buzzerControl(stm_data, 10, 10);
        }
        break;

        //*
        /// -----------------------------------------------------------------------------
        // case 52:
        //     printf("PERSIAPAN TENDANG BOLA 2\n");
        //     if (ball->is_caught == 0)
        //     {
        //         motion->catchingBall(100, 90);
        //         break;
        //     }

        //     if (motion->ruckigToPoint(70, 100, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 170, 100))
        //     {
        //         game_sub_status->value = 55;
        //         // buzzerControl(stm_data, 10, 10);
        //     }

        //     if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 50)
        //     {
        //         game_sub_status->value = 55;
        //     }
        //     break;
        //     /// -----------------------------------------------------------------------------

    case 56:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 150;
        robot3_posisi_y = 100;

        game_sub_status->value = 6;
        break;

    case 6:
        printf("PERSIAPAN TENDANG BOLA 2\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->motionToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 170, 100)) {
            game_sub_status->value = 7;
            // buzzerControl(stm_data, 10, 10);
        }

        if (pythagoras(final_pose->x, final_pose->y, pos_x_dapat_bola, pos_y_dapat_bola) > 100 && fabs(error_sudut_hadap) < 1.4) {
            game_sub_status->value = 7;
        }
        game_sub_status->resetUptimeTimeout();
        break;
    case 7:
        motion->manualMotion(0, 0, 0);

        game_sub_status->timeout(71, 0.1);
        break;
    case 71:
        printf("TENDANG BOLA 2\n");
        // motion->manualMotion(0, 0, 0);
        motion->motionToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(robot2_posisi_x, robot2_posisi_y, *final_pose), 0, 45);
        if (ball->is_caught == 0) {
            game_sub_status->value = 8;
            *robot_condition = 11;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 11.5);
            // buzzerControl(stm_data, 10, 10);
        }
        break;
    case 8:
        if (obs_kiper == 1 && obs_pinggir == 3) {
            // game_sub_status->resetUptimeTimeout();
            // game_sub_status->value = 90;
            motion->manualMotion(0, 0, 0);
            game_sub_status->value = 999;
        } else {
            motion->manualMotion(0, 0, 0);
            game_sub_status->value = 999;
        }

        break;
    case 90:
        motion->manualMotion(0, 0, 0);
        game_sub_status->timeout(91, 0.1);
        break;
        //* oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
    case 91:
        robot3_posisi_x = 50;
        robot3_posisi_y = 100;
        game_sub_status->value = 911;
        break;
    case 911:
        printf("PERSIAPAN TERIMA BOLA 2\n");

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(700, 100, *final_pose), 150, 120)) {
            game_sub_status->value = 92;
        }
        break;

    case 92:
        printf("TUNGGU TERIMA BOLA 2\n");

        motion->manualMotion(0, 0, 0);
        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 93;
        }
        break;

    case 93:
        printf("TERIMA BOLA 2\n");

        *robot_condition = 0;
        if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
            game_sub_status->value = 94;
            pos_x_dapat_bola = final_pose->x;
            pos_y_dapat_bola = final_pose->y;
        }
        break;
    case 94:
        motion->manualMotion(0, 0, 0);
        robot3_posisi_x = 120;
        robot3_posisi_y = 160;
        game_sub_status->value = 944;
        break;

    case 944:
        printf("GIRING BOLA 2\n");

        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        if (motion->ruckigToPoint(robot3_posisi_x, robot3_posisi_y, angleToTarget(400, 0, *final_pose), 100, 70)) {
            game_sub_status->value = 95;
        }

        break;

    case 95:
        printf("PERSIAPAN TENDANG GOAL\n");
        if (ball->is_caught == 0) {
            motion->catchingBall(100, 90);
            break;
        }

        error_sudut_hadap = angleToTarget(400, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->ruckigToPoint(final_pose->x, final_pose->y, angleToTarget(400, 0, *final_pose), 100, 70)) {
            game_sub_status->value = 96;
            motion->manualMotion(0, 0, 0);
        }

        if (fabs(error_sudut_hadap) < 2.1) {
            game_sub_status->value = 96;
        }
        break;

        //*

    case 96:
        printf("TENDANG GOAL\n");

        motion->manualMotion(0, 0, 0);
        if (ball->is_caught == 0) {
            game_sub_status->value = 99;
        } else {
            kickerControl(stm_data, THROTTLE_AIM, 18);
        }

        break;
    case 99:
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 999;
        break;
    case 999:
        // game_status->value = 'K';
        // motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 100);
        if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
            motion->manualMotion(0, 0, 0);
        }

        break;
    }
}

void Defender::gameStyleE() const
{
    printf("Defender::gameStyle E() %d || %d %d || %d %.2f %.2f\n", game_sub_status->value, shared_robot_data[2]->robot_condition, shared_robot_data[3]->robot_condition, ball->status, ball->pos.x, ball->pos.y);

    static float pos_x_dapat_bola = 0;
    static float pos_y_dapat_bola = 0;
    static float error_sudut_hadap = 0;

    float pos_x_teman = shared_robot_data[2]->pose.x;
    float pos_y_teman = shared_robot_data[2]->pose.y;

    static float arah_goal_x = 0;

    static float robot2_posisi_x = 0;
    static float robot2_posisi_y = 0;

    static float robot3_posisi_x = 0;
    static float robot3_posisi_y = 0;

    robot2_posisi_x = pos_x_teman;
    robot2_posisi_y = pos_y_teman - 1;

    int8_t obs_kiper = bs_data->obs_bs_regional[0];
    int8_t obs_tengah = bs_data->obs_bs_regional[1];
    int8_t obs_pinggir = bs_data->obs_bs_regional[2];

    game_sub_status->reentry(0, 0.5);

    if (bs_data->prev_style != bs_data->style) {
        game_sub_status->value = 0;
    }

    switch (game_sub_status->value) {
        // case 0:
        //     stm_data->global_kicker_position = 800;
        //     *robot_condition = 0;

        //     printf("AMBIL BOLA\n");
        //     if (motion->ruckigToPoint(150, 540, -10, 250, 200))
        //     {
        //         *robot_condition = 0;
        //         game_sub_status->value = 101;
        //         // buzzerControl(stm_data, 10, 10);
        //     }

        //     if (pythagoras(final_pose->x, final_pose->y, 150, 540) < 100)
        //     {

        //         game_sub_status->value = 101;
        //         *robot_condition = 0;
        //         // buzzerControl(stm_data, 10, 10);
        //     }
        //     break;

    case 0:
        //* TENDANG GOAL  =========================================
        // game_sub_status->value = 10;
        //* UMPAN ===================================================
        game_sub_status->value = 100;

    case 100:

        printf("AMBIL BOLA\n");
        *robot_condition = 0;
        if (motion->ruckigToPoint(320, 645, -7, 250, 200)) {
            *robot_condition = 0;
            game_sub_status->value = 1;
            // buzzerControl(stm_data, 10, 10);
        }

        if (pythagoras(final_pose->x, final_pose->y, 320, 645) < 50) {
            game_sub_status->value = 1;
            *robot_condition = 0;
            // buzzerControl(stm_data, 10, 10);
        }
        break;
    case 1:
        printf("CATCHING BALL\n");
        *robot_condition = 0;

        if (motion->catchingBall(80, 50)) {
            game_sub_status->value = 2;
        }
        break;
    case 2:
        if (motion->pivotMotion(angleToTarget(760, 460, *final_pose), 90)) {
            game_sub_status->value = 4;
        }

        error_sudut_hadap = angleToTarget(760, 460, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (fabs(error_sudut_hadap) < 5) {
            game_sub_status->value = 4;
        }

        break;
    case 4:
        printf("TENDANG BOLA 1\n");
        motion->manualMotion(0, 0, 0);

        if (ball->is_caught == 2) {
            kickerControl(stm_data, NO_AIM, 8.5);
            game_sub_status->value = 16;
            *robot_condition = 11;
            // buzzerControl(stm_data, 10, 10);
        }
        break;

    //*==============================================
    case 10:

        if (obs_kiper == 1) {
            arah_goal_x = 350;
        } else if (obs_kiper == 2) {
            arah_goal_x = 330;
        } else if (obs_kiper == 3) {
            arah_goal_x = 430;
        }

        game_sub_status->value = 11;
        break;
    case 11:
        stm_data->global_kicker_position = 150;

        if (motion->ruckigToPoint(50, 460, angleToTarget(400, 600, *final_pose), 250, 100)) {
            game_sub_status->value = 12;
        }
        break;

    case 12:
        stm_data->global_kicker_position = 150;

        motion->manualMotion(0, 0, 0);
        if (shared_robot_data[2]->robot_condition == 11) {
            game_sub_status->value = 13;
        }
        break;

    case 13:
        stm_data->global_kicker_position = 150;

        if (motion->receiveBall3(shared_robot_data[2]->pose.x, shared_robot_data[2]->pose.y, 100, 90)) {
            game_sub_status->value = 14;
        }
        break;

    case 14:
        stm_data->global_kicker_position = 150;

        error_sudut_hadap = angleToTarget(arah_goal_x, 0, *final_pose) - final_pose->theta;

        if (error_sudut_hadap < -180) {
            error_sudut_hadap += 360;
        } else if (error_sudut_hadap > 180) {
            error_sudut_hadap -= 360;
        }

        if (motion->motionToPoint(40, 460, angleToTarget(arah_goal_x, 0, *final_pose), 200, 155)) {
            game_sub_status->value = 15;
        }

        if ((pythagoras(40, 460, final_pose->x, final_pose->y) < 50) && fabs(error_sudut_hadap) < 2) {
            game_sub_status->value = 15;
        }

        break;

    case 15:
        motion->motionAngleControl(0, 0, angleToTarget(arah_goal_x, 0, *final_pose), 15);

        if (ball->is_caught == 0) {
            game_sub_status->value = 16;
        } else {
            if (obs_kiper == 3) {
                kickerControl(stm_data, KICKER_POSITION_AIM | THROTTLE_AIM, 21, 150);
            } else {
                kickerControl(stm_data, KICKER_POSITION_AIM | THROTTLE_AIM, 19, 150);
            }
        }
        break;

    case 16:
        // motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 100);
        motion->manualMotion(0, 0, 0);
        game_sub_status->value = 17;
        break;
    case 17:
        if (motion->ruckigToPoint(BASE_2.x, BASE_2.y, BASE_2.theta, 270, 150)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    }
}
