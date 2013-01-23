DROP TABLE IF EXISTS `items`;
CREATE TABLE  `items` (
	`ID` int NOT NULL auto_increment,
	`charID` int NOT NULL,
	`refItemID` int NOT NULL,
	`EnchantLevel` int(1) NOT NULL,
	`Quantity` int NOT NULL,
	`Location` int(1) NOT NULL,
	`Position` int(1) NOT NULL,
	`Durability` int(11) NOT NULL,
	`Phy` int(1) NOT NULL,
	`Mag` int(1) NOT NULL,
	`Dur` int(1) NOT NULL,
	`AP` int(1) NOT NULL,
	`CB` int(1) NOT NULL,
	`PhyReinf` int(1) NOT NULL,
	`MagReinf` int(1) NOT NULL,
	PRIMARY KEY(`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
