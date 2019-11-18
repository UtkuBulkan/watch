#include "mysql_connection.h"
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>

#include "qt/watch_qt_data.h"

struct camera_list_item_t
{
	int id;
	std::string address;
	bool active;
	bool face_detection;
	bool face_recognition;
	bool object_detection;
	bool people_counter;
	bool gender_prediction;
	bool age_prediction;
	bool heat_map_estimation;
	bool record_detections_as_output_file;
};

class DBConnection
{
public:
	DBConnection(const std::string& server, const std::string& user, const std::string& pass);
	~DBConnection();
	void add_camera(std::string camera_address, CameraSettingsData &camera_settings_data);
	void update_camera(std::string camera_address, CameraSettingsData &camera_settings_data);
	void get_camera_list(std::vector<camera_list_item_t> &camera_list);
	bool check_camera_exists(std::string camera_address);

	void add_face(int id, int camera_id, int timestamp, unsigned char *image, unsigned int image_size, std::string auto_assigned, std::string user_assigned, int previously_detected);

protected:
	std::string m_server; //eg: 192.168.1.1 or localhost:3306
	std::string m_user;
	std::string m_pass;
	std::string m_db;
	int m_port;

private:
	void preparetables();
	sql::mysql::MySQL_Driver *driver;
	sql::Connection *con;
	sql::Statement *statement;
	sql::PreparedStatement *prepare_statement;
};
