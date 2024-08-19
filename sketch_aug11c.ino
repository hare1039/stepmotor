#include <ArduinoSTL.h>
#include <Stepper.h>


constexpr arduino_time_t defined_rotate_duration  = 5000;     // milliseconds; variable
constexpr arduino_time_t defined_pause_duration   = 1000 * 3; // milliseconds; variable
constexpr int            defined_number_of_cycles = 40;       // 次; variable
constexpr int            defined_steps            = 20;       // step per revolution; variable
constexpr int            defined_stepper_speed    = 500;      // rpm; variable
constexpr int            button_start_port        = 8;        // D8; variable => power on --> Run program, power off --> disable
constexpr int            button_right_port        = 9;        // D9; variable => power on --> Run conti. right; power off --> unuseful

using arduino_time_t = unsigned long int;
auto now() -> arduino_time_t { return millis(); }

enum class move_mode: char
{
    pause,
    rotate_left,
    rotate_right,
};

using execution_plan = std::vector<move_mode>;

execution_plan normal_plan;
execution_plan move_right_plan;

class state
{
    static constexpr int steps_ = defined_steps;
    Stepper stepper_;
    arduino_time_t last_run_ = now();
    arduino_time_t execution_duration_ = 0;
    int state_index_ = 0;
    execution_plan states_;

public:
    state(): stepper_{steps_, 2, 4, 3, 5} {
        stepper_.setSpeed(defined_stepper_speed); // rpm
    }

    void set_execution_plan(execution_plan const & state)
    {
        if (states_ != state)
        {
            // reset the whole state
            states_ = state;
            state_index_ = -1;
            last_run_ = 0;
        }
    }

    void execute()
    {
        if (states_.empty())
            return;
        if (now() - last_run_ >= execution_duration_)  // execution finished
        {
            int const next_index = state_index_ + 1;
            if (next_index >= states_.size())
                return;
			state_index_ = next_index;
            switch (states_[state_index_])
            {
            case move_mode::pause:
                execution_duration_ = defined_pause_duration;
                break;
            case move_mode::rotate_left:
            case move_mode::rotate_right:
                execution_duration_ = defined_rotate_duration;
                break;
            }
            last_run_ = now();
        }
        else // in execution
        {
            switch (states_[state_index_])
            {
            case move_mode::rotate_left:
                stepper_.step(1);
                break;
            case move_mode::rotate_right:
                stepper_.step(-1);
                break;
            case move_mode::pause:
                break;
            }
        }
    }
};

state motor;

void setup()
{
    Serial.begin(9600);
    pinMode(button_start_port, INPUT);
    pinMode(button_right_port, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < defined_number_of_cycles; i++)
    {
        normal_plan.push_back(move_mode::rotate_left);
        normal_plan.push_back(move_mode::pause);
    }
    for (int i = 0; i < defined_number_of_cycles; i++)
        move_right_plan.push_back(move_mode::rotate_right);
}

void loop()
{
    int const power_on = digitalRead(button_start_port);
    if (power_on)
    {
        int const right = digitalRead(button_right_port);
        if (right)
            motor.set_execution_plan(move_right_plan);
        else
            motor.set_execution_plan(normal_plan);
        motor.execute();
    }
}
