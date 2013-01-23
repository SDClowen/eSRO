DROP TABLE IF EXISTS `friend_group`;
CREATE TABLE `friend_group` (
	`refgroupID` INTEGER NOT NULL AUTO_INCREMENT,
	`charID` INTEGER NOT NULL,
	`groupID` INTEGER NOT NULL,
	`Name` varchar(50) NOT NULL,
	PRIMARY KEY (`refgroupID`)
);

