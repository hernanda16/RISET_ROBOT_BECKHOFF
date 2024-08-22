#ifndef PID_H_
#define PID_H_

#include <chrono>

class PID {
private:
    float Kp;
    float Ki;
    float Kd;
    float min_out;
    float max_out;
    float min_integral;
    float max_integral;
    float proportional;
    float integral;
    float derivative;
    float last_error;
    float output_speed;
    std::chrono::system_clock::time_point last_call;

public:
    PID(float Kp, float Ki, float Kd)
    {
        this->Kp = Kp;
        this->Ki = Ki;
        this->Kd = Kd;
    }

    float calculate(float error, float minmax)
    {
        std::chrono::high_resolution_clock::time_point t_now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = t_now - this->last_call;
        if (elapsed_seconds.count() > 2) {
            this->integral = 0;
            this->last_error = 0;
        }
        this->last_call = std::chrono::high_resolution_clock::now();

        this->min_out = this->min_integral = -minmax;
        this->max_out = this->max_integral = minmax;

        this->proportional = this->Kp * error;
        this->integral += this->Ki * error;
        this->derivative = this->Kd * (error - this->last_error);

        this->last_error = error;

        if (this->integral > this->max_integral)
            this->integral = this->max_integral;
        else if (this->integral < this->min_integral)
            this->integral = this->min_integral;

        this->output_speed = this->proportional + this->integral + this->derivative;

        if (this->output_speed > this->max_out)
            this->output_speed = this->max_out;
        else if (this->output_speed < this->min_out)
            this->output_speed = this->min_out;
        return this->output_speed;
    }

    void reset()
    {
        this->integral = 0;
        this->last_error = 0;
    }
};

#endif