#ifndef GLFWPP_GLFWPP_H
#define GLFWPP_GLFWPP_H

#include <GLFW/glfw3.h>

#include "error.h"
#include "event.h"
#include "joystick.h"
#include "monitor.h"
#include "native.h"
#include "window.h"

namespace glfw
{
    namespace impl
    {
        void errorCallback(int errorCode, const char* what)
        {
            switch(errorCode)
            {
                case GLFW_NOT_INITIALIZED:
                    throw NotInitializedError(what);
                case GLFW_NO_CURRENT_CONTEXT:
                    throw NoCurrentContextError(what);
                case GLFW_OUT_OF_MEMORY:
                    throw OutOfMemoryError(what);
                case GLFW_API_UNAVAILABLE:
                    throw APIUnavailableError(what);
                case GLFW_VERSION_UNAVAILABLE:
                    throw VersionUnavailableError(what);
                case GLFW_PLATFORM_ERROR:
                    throw PlatformError(what);
                case GLFW_FORMAT_UNAVAILABLE:
                    throw FormatUnavailableError(what);
                case GLFW_NO_WINDOW_CONTEXT:
                    throw NoWindowContextError(what);
                default:
                    throw Error(what);
            }
        }

        void monitorCallback(GLFWmonitor* monitor, int eventType)
        {
            monitorEvent(Monitor{monitor}, MonitorEventType{eventType});
        }

        void joystickCallback(int jid, int eventType)
        {
            joystickEvent(Joystick{(decltype(Joystick::Joystick1))jid}, (JoystickEvent)eventType);
        }
    }  // namespace impl

    struct InitHints
    {
        bool cocoaChdirResources = true;
        bool cocoaMenubar = true;

        void apply() const
        {
            glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, cocoaChdirResources);
            glfwInitHint(GLFW_COCOA_MENUBAR, cocoaMenubar);
        }
    };

    struct GLFWLibrary
    {
    private:
        GLFWLibrary() = default;

    public:
        ~GLFWLibrary()
        {
            glfwTerminate();
        }

        [[nodiscard]] friend GLFWLibrary init()
        {
            glfwSetErrorCallback(impl::errorCallback);

            glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, false);
            if(!glfwInit())
            {
                throw glfw::Error("Could not initialize GLFW");
            }

            glfwSetMonitorCallback(impl::monitorCallback);
            glfwSetJoystickCallback(impl::joystickCallback);

            return {};
        }
    };

    [[nodiscard]] GLFWLibrary init();

    [[nodiscard]] Version getVersion()
    {
        Version version;
        glfwGetVersion(&version.major, &version.minor, &version.revision);
        return version;
    }

    [[nodiscard]] const char* getVersionString()
    {
        return glfwGetVersionString();
    }

    [[nodiscard]] bool rawMouseMotionSupported()
    {
        return glfwRawMouseMotionSupported();
    }

    void setClipboardString(const char* content)
    {
        glfwSetClipboardString(nullptr, content);
    }

    [[nodiscard]] const char* getClipboardString()
    {
        return glfwGetClipboardString(nullptr);
    }

    [[nodiscard]] bool extensionSupported(const char* extensionName)
    {
        return glfwExtensionSupported(extensionName);
    }

    using GLProc = GLFWglproc;
    [[nodiscard]] GLProc getProcAddress(const char* procName)
    {
        return glfwGetProcAddress(procName);
    }

    namespace vulkan
    {
        [[nodiscard]] bool supported()
        {
            return glfwVulkanSupported();
        }

        [[nodiscard]] std::vector<const char*> getRequiredInstanceExtensions()
        {
            unsigned count;
            auto pExtensionNames = glfwGetRequiredInstanceExtensions(&count);

            std::vector<const char*> extensionNames;
            extensionNames.reserve(count);
            for(int i = 0; i < count; ++i)
            {
                extensionNames.push_back(pExtensionNames[i]);
            }
            return extensionNames;
        }
        using VkProc = GLFWvkproc;
#if defined(VK_VERSION_1_0)
        [[nodiscard]] VkProc getInstanceProcAddress(VkInstance instance, const char* procName)
        {
            return glfwGetInstanceProcAddress(instance, procName);
        }

        [[nodiscard]] bool getPhysicalDevicePresentationSupport(
                VkInstance instance,
                VkPhysicalDevice device,
                uint32_t queueFamily)
        {
            return glfwGetPhysicalDevicePresentationSupport(instance, device, queueFamily);
        }
#endif  // VK_VERSION_1_0

#ifdef VULKAN_HPP
        [[nodiscard]] VkProc getInstanceProcAddress(const vk::Instance& instance, const char* procName)
        {
            return getInstanceProcAddress(static_cast<VkInstance>(instance), procName);
        }
        [[nodiscard]] bool getPhysicalDevicePresentationSupport(
                const vk::Instance& instance,
                const vk::PhysicalDevice& device,
                uint32_t queueFamily)
        {
            return getPhysicalDevicePresentationSupport(static_cast<VkInstance>(instance), static_cast<VkPhysicalDevice>(device), queueFamily);
        }
#endif  // VULKAN_HPP
    }  // namespace vulkan

    namespace timer
    {
        [[nodiscard]] double getTime()
        {
            return glfwGetTime();
        }

        void setTime(double time)
        {
            glfwSetTime(time);
        }

        [[nodiscard]] uint64_t getValue()
        {
            return glfwGetTimerValue();
        }

        [[nodiscard]] uint64_t getFrequency()
        {
            return glfwGetTimerFrequency();
        }
    }  // namespace timer
}  // namespace glfw

#endif  // GLFWPP_GLFWPP_H