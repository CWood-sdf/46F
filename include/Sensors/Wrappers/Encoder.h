#ifndef ENCODER_H
#define ENCODER_H
#include "vex.h"
#include "v5_apitypes.h"
#include "vex_device.h"
#include "vex_triport.h"
//Helpful information:
//V5_DeviceType (v5_apitypes.h)
//devices (vex_device.h)
//triport and __tridevice (vex_triport.h)
//Will prolly need a different hack for triport devices rather than the normal ones
class Encoder {
    V5_DeviceType encoderType = V5_DeviceType::kDeviceTypeMotorSensor;
    void* encoder;
    static inline devices device = devices();
public:
    //A struct that gets the type of the encoder from the V5_DeviceType
    template<V5_DeviceType DeviceType>
    struct GetType {
        typedef void type;
    };
    template<V5_DeviceType DeviceType>
    typename Encoder::GetType<DeviceType>::type* getEncoder() {
        return (typename Encoder::GetType<DeviceType>::type*)encoder;
    }
    
};
template<>
struct Encoder::GetType<V5_DeviceType::kDeviceTypeMotorSensor> {
    typedef motor type;
};
#endif