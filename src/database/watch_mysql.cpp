#include "watch_mysql.h"
#include <exception>
#include <syslog.h>


DBConnection::DBConnection(const std::string& server, const std::string& user, const std::string& pass)
: m_server(server), m_user(user), m_pass(pass), m_port(3306)
{
	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect(server, user, pass);

	try {
		con->setSchema("watch");
	} catch (std::exception &e) {
		syslog(LOG_NOTICE, "No database is available in MySQL with name watch");
		statement = con->createStatement();
		delete statement;
		statement->execute("CREATE DATABASE watch");
		con->setSchema("watch");
	}
	syslog(LOG_NOTICE, "MySQL database is created with title : %s", con->getSchema().c_str());
	preparetables();
}

void DBConnection::preparetables()
{
	try {
		syslog(LOG_NOTICE, "Creating Tables");
		statement = con->createStatement();
		statement->execute("CREATE TABLE IF NOT EXISTS cameras (id INT AUTO_INCREMENT PRIMARY KEY, address VARCHAR(100), active INT, face_detection_flag INT, face_recognition_flag INT, object_detection_flag INT, people_counter_flag INT, gender_prediction_flag INT, age_prediction_flag INT, heat_map_flag INT, record_detections_as_output_file INT)");
		delete statement;
		statement = con->createStatement();
		statement->execute("CREATE TABLE IF NOT EXISTS recorded_files(id INT, address VARCHAR(200), record_timestamp VARCHAR(50))");
		delete statement;
		statement = con->createStatement();
		statement->execute("CREATE TABLE IF NOT EXISTS detected_faces(id INT, auto_assigned_label VARCHAR(100), user_assigned_label VARCHAR(100))");
		delete statement;
	} catch (std::exception &e) {
		syslog(LOG_NOTICE, "Failed to create tables for watch database.");
	}
	syslog(LOG_NOTICE, "Tables were created succesfully.");
}

void DBConnection::get_camera_list(std::vector<camera_list_item_t> &camera_list)
{
	syslog(LOG_NOTICE, "DBConnection::get_camera_list Start");
	statement = con->createStatement();
	sql::ResultSet *res = statement->executeQuery("SELECT * FROM cameras");
	while(res->next()) {
		std::cout << res->getString("address") << std::endl;
		camera_list_item_t camera_list_item;
		camera_list_item.id = res->getInt("id");
		camera_list_item.address = res->getString("address");
		camera_list_item.active = res->getInt("active");
		camera_list_item.face_detection = res->getInt("face_detection_flag");
		camera_list_item.face_recognition = res->getInt("face_recognition_flag");
		camera_list_item.object_detection = res->getInt("object_detection_flag");
		camera_list_item.people_counter = res->getInt("people_counter_flag");
		camera_list_item.gender_prediction = res->getInt("gender_prediction_flag");
		camera_list_item.age_prediction = res->getInt("age_prediction_flag");
		camera_list_item.heat_map_estimation = res->getInt("heat_map_flag");
		camera_list_item.record_detections_as_output_file = res->getInt("record_detections_as_output_file");
		camera_list.push_back(camera_list_item);
	}
	delete statement;
	syslog(LOG_NOTICE, "DBConnection::get_camera_list End");
}

void DBConnection::add_camera(std::string camera_address, CameraSettingsData &camera_settings_data)
{
	try {
		prepare_statement = con->prepareStatement("INSERT INTO cameras (id, address, active, face_detection_flag, face_recognition_flag, object_detection_flag, people_counter_flag, gender_prediction_flag, age_prediction_flag, heat_map_flag, record_detections_as_output_file) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		prepare_statement->setInt(1, 0);
		prepare_statement->setString(2, camera_address);
		prepare_statement->setInt(3, camera_settings_data.active);
		prepare_statement->setInt(4, camera_settings_data.face_detection);
		prepare_statement->setInt(5, camera_settings_data.face_recognition);
		prepare_statement->setInt(6, camera_settings_data.object_detection);
		prepare_statement->setInt(7, camera_settings_data.people_counter);
		prepare_statement->setInt(8, camera_settings_data.gender_prediction);
		prepare_statement->setInt(9, camera_settings_data.age_prediction);
		prepare_statement->setInt(10, camera_settings_data.heat_map_estimation);
		prepare_statement->setInt(11, camera_settings_data.record_detections_as_output_file);
		prepare_statement->executeUpdate();
		delete prepare_statement;
	} catch (std::exception &e) {
		syslog(LOG_NOTICE, "Failed to add a new camera information into database.");
	}
}

void DBConnection::update_camera(std::string camera_address, CameraSettingsData &camera_settings_data)
{
	try {
		prepare_statement = con->prepareStatement("UPDATE cameras SET active=?,face_detection_flag=?, face_recognition_flag=?, object_detection_flag=?, people_counter_flag=?, gender_prediction_flag=?, age_prediction_flag=?, heat_map_flag=?, record_detections_as_output_file=? WHERE address=?");
		prepare_statement->setInt(1, camera_settings_data.active);
		prepare_statement->setInt(2, camera_settings_data.face_detection);
		prepare_statement->setInt(3, camera_settings_data.face_recognition);
		prepare_statement->setInt(4, camera_settings_data.object_detection);
		prepare_statement->setInt(5, camera_settings_data.people_counter);
		prepare_statement->setInt(6, camera_settings_data.gender_prediction);
		prepare_statement->setInt(7, camera_settings_data.age_prediction);
		prepare_statement->setInt(8, camera_settings_data.heat_map_estimation);
		prepare_statement->setInt(9, camera_settings_data.record_detections_as_output_file);
		prepare_statement->setString(10, camera_address);
		prepare_statement->executeUpdate();
		delete prepare_statement;
	} catch (std::exception &e) {
		syslog(LOG_NOTICE, "Failed to update the camera information into database.");
	}
}

bool DBConnection::check_camera_exists(std::string camera_address)
{
	statement = con->createStatement();
	sql::ResultSet *res = statement->executeQuery("SELECT * FROM cameras");
	while(res->next()) {
		if(camera_address == res->getString("address"))
			return true;
	}
	return false;
}

DBConnection::~DBConnection()
{
	delete statement;
	delete con;
}
