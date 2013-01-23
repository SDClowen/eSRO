DROP TABLE IF EXISTS `refmappingshopgroup`;
CREATE TABLE `refmappingshopgroup` (
	`EntryFlag` INTEGER(1) NOT NULL,
	`ItemFlag` INTEGER(1) NOT NULL,
	`Group` VARCHAR(50) NOT NULL,
	`Shop` VARCHAR(50) NOT NULL
);
