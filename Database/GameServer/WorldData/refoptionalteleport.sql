DROP TABLE IF EXISTS `refoptionalteleport`;
CREATE TABLE `refoptionalteleport` (
	`ID` INTEGER NOT NULL,
	`Zone` INTEGER NOT NULL,
	`X` float NOT NULL,
	`Z` float NOT NULL,
	`Y` float NOT NULL,
	`Location` INTEGER NOT NULL,
	`Restricted` INTEGER NOT NULL,
	`MinLv` INTEGER NOT NULL,
	`MaxLv` INTEGER NOT NULL
);

