#include <dlt/dlt.h>
#include <signal.h>
#include <unistd.h>

DLT_DECLARE_CONTEXT(sample_context);

static volatile sig_atomic_t running = 1;

static void stop(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int main(void)
{
    unsigned int counter = 0;

    signal(SIGINT, stop);
    signal(SIGTERM, stop);

    DLT_REGISTER_APP("RPI5", "Raspberry Pi 5 DLT sample");
    DLT_REGISTER_CONTEXT(sample_context, "MAIN", "Sample application");

    while (running) {
        DLT_LOG(sample_context, DLT_LOG_INFO,
                DLT_STRING("Raspberry Pi sample heartbeat"),
                DLT_UINT(counter++));
        sleep(1);
    }

    DLT_UNREGISTER_CONTEXT(sample_context);
    DLT_UNREGISTER_APP();
    return 0;
}
