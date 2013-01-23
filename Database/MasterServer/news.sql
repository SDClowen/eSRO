CREATE TABLE `news` (
  `id` integer UNSIGNED NOT NULL AUTO_INCREMENT,
  `title` varchar(20)  NOT NULL,
  `message` TEXT  NOT NULL,
  `date` datetime  NOT NULL,
  PRIMARY KEY (`id`)
)
ENGINE = MyISAM;
