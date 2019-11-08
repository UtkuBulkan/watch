apt install mysql-server
apt-get install libmysqlclient-dev
apt-get install libmysqlcppconn-dev
apt install php-mysql
apt install php7.0

sudo apt-get install php libapache2-mod-php
sudo a2enmod mpm_prefork && sudo a2enmod php7.0
sudo service apache2 restart

mysql -u root -p
# GRANT ALL PRIVILEGES ON *.* TO 'utku'@'localhost' IDENTIFIED BY 'utku';