#include <Client.hpp>
#include <string>

class App : public Network::Client
{
public:
    App();

    ~App();

protected:
    void onServerAccepted() override;
    void onServerPing(double timestamp) override;
    void onRegistrationAnswer(Utility::RegistrationCodes registrationCode) override;
};
