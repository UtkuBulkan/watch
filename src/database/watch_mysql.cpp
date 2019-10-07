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
		statement->execute("CREATE TABLE IF NOT EXISTS cameras (id INT AUTO_INCREMENT PRIMARY KEY, address VARCHAR(100), is_record_as_output INT)");
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
}

void DBConnection::get_camera_list(std::vector<camera_list_item_t> &camera_list)
{
	statement = con->createStatement();
	sql::ResultSet *res = statement->executeQuery("SELECT * FROM cameras");
	while(res->next()) {
		std::cout << res->getString("address") << std::endl;
		camera_list_item_t camera_list_item;
		camera_list_item.id = res->getInt("id");
		camera_list_item.address = res->getString("address");
		camera_list_item.is_record_as_output = res->getInt("is_record_as_output");
		camera_list.push_back(camera_list_item);
	}
	delete statement;
}

void DBConnection::add_camera(std::string camera_address)
{
	try {
		prepare_statement = con->prepareStatement("INSERT INTO cameras (id, address, is_record_as_output) VALUES (?, ?, ?)");
		prepare_statement->setInt(1, 0);
		prepare_statement->setString(2, camera_address);
		prepare_statement->setInt(3, 0);
		prepare_statement->executeUpdate();
		delete prepare_statement;
	} catch (std::exception &e) {
		syslog(LOG_NOTICE, "Failed to add a new camera information into database.");
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

void DBConnection::logLastError()
{
}

int DBConnection::sql()
{
}

