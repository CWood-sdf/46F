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
    // encoder sdfsd = encoder(Brain.ThreeWirePort.A);
    // const V5_DeviceType encoderType = V5_DeviceType::kDeviceTypeMotorSensor;
    std::function<double(rotationUnits)> getValue;
    std::function<void()> resetter;
    // static inline devices device = devices();
public:
    // SFINAE test
    template <typename T>
    class HasResetPosition
    {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename C> static YesType& test(decltype(&C::resetPosition));
        template <typename C> static NoType& test(...);


    public:
        enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
    };
    //The constructor, only enable if the type has been mapped
    template<class Sensor>
    Encoder(Sensor* e, typename std::enable_if<HasResetPosition<Sensor>::value, std::string>::type sdf = "") {
        getValue = [e](rotationUnits units) {
            return e->position(units);
        };
        resetter = [e]() {
            e->resetPosition();
        };
    }
    template<class Sensor>
    Encoder(Sensor* e, typename std::enable_if<!HasResetPosition<Sensor>::value, std::string>::type sdf = "") {
        getValue = [e](rotationUnits units) {
            return e->position(units);
        };
        resetter = [e]() {
            e->resetRotation();
        };
    }
    template<class Sensor>
    Encoder(Sensor& e) : Encoder(&e) {
    }
    Encoder() {
        getValue = [](rotationUnits units) {
            return 0;
        };
        resetter = [](void) {
            
        };
    }
    //Make the position method
    double position(rotationUnits units) {
        return getValue(units);
    }
    void resetPosition() {
        resetter();
    }
    Encoder& operator=(const Encoder& other) {
        getValue = other.getValue;
        return *this;
    }
    Encoder& operator=(Encoder&& other) {
        getValue = other.getValue;
        return *this;
    }
};
// //Matches kDeviceTypeMotorSensor to motor encoders
// template<>
// struct Encoder::GetType<V5_DeviceType::kDeviceTypeMotorSensor> {
//     typedef motor type;
// };
// template<>
// struct Encoder::GetDeviceType<motor> {
//     static constexpr V5_DeviceType deviceType = V5_DeviceType::kDeviceTypeMotorSensor;
// };
// //Matches kDeviceTypeAbsEncSensor to rotation sensor
// template<>
// struct Encoder::GetType<V5_DeviceType::kDeviceTypeAbsEncSensor> {
//     typedef rotation type;
// };
// template<>
// struct Encoder::GetDeviceType<rotation> {
//     static constexpr V5_DeviceType deviceType = V5_DeviceType::kDeviceTypeAbsEncSensor;
// };
// //Matches kDeviceTypeUndefinedSensor to three wire encoder
// template<>
// struct Encoder::GetType<V5_DeviceType::kDeviceTypeUndefinedSensor> {
//     typedef encoder type;
// };
// template<>
// struct Encoder::GetDeviceType<encoder> {
//     static constexpr V5_DeviceType deviceType = V5_DeviceType::kDeviceTypeUndefinedSensor;
// };
#ifndef NO_MAKE
motor* sdfsdf = nullptr;
Encoder* globalEnc = new Encoder(sdfsdf);
encoder* sdfdfsd = nullptr;
Encoder* globalEnc2 = new Encoder(sdfdfsd);
#endif
#endif  // ENCODER_H