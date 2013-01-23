CREATE TABLE `accounts` (
  `id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `username` varchar(20)  NOT NULL DEFAULT '0',
  `password` varchar(50)  NOT NULL DEFAULT '0',
  `Email` varchar(255) DEFAULT NULL,
  `created_date` datetime  NOT NULL,
  `last_login_date` datetime  NOT NULL,
  `last_login_ip` varchar(16)  NOT NULL,
  `country` varchar(64)  NOT NULL,
  `gender` varchar(1)  NOT NULL DEFAULT 'N',
  `birth` datetime  NOT NULL,
  `name` varchar(64)  NOT NULL,
  `access_level` int(1)  NOT NULL DEFAULT 0,
  `security_level` int(2)  NOT NULL DEFAULT 0,
  `silk` int  NOT NULL DEFAULT 0,
  `state` int(1)  NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
)
ENGINE = MyISAM;
