#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Pipeline {
public:
    Pipeline();
    ~Pipeline();
    void associate_with_camera();
    void connect_to_camera();
    void disconnect_from_camera();
    void join();
    void drop();
    void loop();
private:
};

