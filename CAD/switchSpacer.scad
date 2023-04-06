module keySpacer(){
 
 difference(){
 translate([0+1.5,0,])cube([ 27.8*2+3,27.8*2, 4.8], center = true);
 translate([0,0,0])cube([ 27.8*2+3,27.8*2, 4.8], center = true);
 }
 
 difference(){
 translate([0+1.5,0,+2.4])cube([ 27.8*2+3,27.8*2, 4.8], center = true);
 
 translate([0,0,0])cube([16,15,10],center=true);
 translate([18,0,0])cube([16,15,10],center=true);
 translate([0,18,0])cube([16,15,10],center=true);
 translate([-18,0,0])cube([16,15,10],center=true);
 translate([0,-18,0])cube([16,15,10],center=true);

 }

}

keySpacer();