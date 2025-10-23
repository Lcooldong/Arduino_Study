#ifndef __ROBOT_MOTOR_H__
#define __ROBOT_MOTOR_H__

#define TMR_57_F       // Axis 5, Axis 6, Axis 7
// #define TMR_85_F
// #define TMR_100_F   // Axis 3, Axis 4

// #define TBM_7615
// #define TBM_12913   // Axis 1, Axis 2
// #define TBM_12941

#ifdef TMR_57_F

#define TMR_57_RATED_VOLTAGE    48
#define TMR_57_RATED_CURRENT   2.6
#define TMR_57_PEAK_CURRENT    5.3

#define TMR_57_MIN_VOLTAGE      24
#define TMR_57_VOLTAGE_LIMIT    48
#define TMR_57_RESISTANCE     0.47 // 470mΩ  -> 520 mΩ measured in CCS
#define TMR_57_POLE_PAIR        10 // 20P -> 10PP (Pole Pair)

#define TMR_57_FREQ           3500 // Under 3500 RPM stable operation




#elif TMR_100_F

#define KV340_MAX_VOLTAGE   33
#define KV340_MIN_VOLTAGE   22
#define KV340_VOLTAGE_LIMIT 24
#define KV340_RESISTANCE    35 // 35mΩ
#define KV340_POLE_PAIR     14

#elif TBM_12913


// #else if TBM_12913

#define TBM_12913_MAX_VOLTAGE   50


#endif


#endif
