DROP TABLE IF EXISTS `guild_alliance`;
CREATE TABLE `guild_alliance` (
	`ID` INTEGER NOT NULL auto_increment,
	`EmblemIndex` INT(2) NOT NULL DEFAULT 0,
	`HeadGuildID` INTEGER NOT NULL,
	PRIMARY KEY (`ID`)
);
