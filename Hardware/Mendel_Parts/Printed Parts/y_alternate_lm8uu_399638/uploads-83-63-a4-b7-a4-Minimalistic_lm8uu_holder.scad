//******************
//Adjustable Variables
//******************

$fn =30;
lmuuOuterD = 15;
ShellSize = 3;
MBoltSize = 3;
MountHolesCenters = 25;
BaseOffset = 0.2;
MountThickness = 5;
HolderWidthMultiplier = 2; 

//******************

//Other Variables Used
MountLength = MountHolesCenters+(MBoltSize*2)+(ShellSize*2);
HolderWidth = MBoltSize+(HolderWidthMultiplier*ShellSize);

difference()
{
	union()
	{
//Outer Cylinder
		translate([0,0,HolderWidth/2])
			cylinder(r=lmuuOuterD/2 +ShellSize,h =HolderWidth,center=true);
//Base Mounting
		translate([MountLength/-2,(lmuuOuterD/2)-BaseOffset-MountThickness,0])
			cube([MountLength,MountThickness,HolderWidth]);
		
	}
//Bottom Cut
	translate([(lmuuOuterD+ShellSize)/-2,(lmuuOuterD/2)-BaseOffset,0])
			cube([lmuuOuterD+(ShellSize),ShellSize*2,HolderWidth]);
	
//Inner Cylinder
	translate([0,0,HolderWidth/2])
		cylinder(r = lmuuOuterD/2,h = HolderWidth+0.1, center = true);
	
//Mounting Hole1
	translate([MountHolesCenters/2,(lmuuOuterD/2)-BaseOffset-(MountThickness/2),HolderWidth/2])
	{
		cube([MBoltSize*2,MountThickness+0.2,MBoltSize],center=true);
	rotate([90,0,0])
		translate([0,0,MountThickness/2])
		cylinder(r=MBoltSize+0.5,h=10);
	}
//Mirror of Mounting Hole1
	mirror([1,0,0]){
		translate([MountHolesCenters/2,(lmuuOuterD/2)-BaseOffset-(MountThickness/2),HolderWidth/2])
		{
			cube([MBoltSize*2,MountThickness+0.2,MBoltSize],center=true);
		rotate([90,0,0])
			translate([0,0,MountThickness/2])
				cylinder(r=MBoltSize+0.5,h=10);
		}
	}

//Blunt the pointy bits
	translate([0,(lmuuOuterD/2)-BaseOffset-ShellSize/2,HolderWidth/2])
			cube([ShellSize*4,ShellSize,HolderWidth], center = true);
}