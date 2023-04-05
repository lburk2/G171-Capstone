module keySpacer(){
 
// difference(){
// translate([-1.75,0,-1.5])cube([21.6, 90/2, 4.8+1.5]);
// translate([0,0,-1.5])cube([50, 90/2, 4.8+1.5]);
// }
 difference(){
 translate([10.8,10.8,+2.4])cube([ 70,70, 4.8], center = true);
 
 translate([10.8,10.8,0])cube([13.3,13.5,10],center=true);
 translate([10.8,32.4,0])cube([13.3,13.5,10],center=true);
 translate([32.4,10.8,0])cube([13.3,13.5,10],center=true);
 translate([-10.8,10.8,0])cube([13.3,13.5,10],center=true);
 translate([10.8,-10.8,0])cube([13.3,13.5,10],center=true);

 }

}

keySpacer();