module keySpacer(){
 
 translate([1.65/2+27.9,0,(4.8+1.65)/2-1.65])cube([ 1.5,27.8*2, 4.8+1.65], center = true);

 
 difference(){
 translate([0+0.5,0,+2.4])cube([ 27.8*2+2,27.8*2, 4.8], center = true);
 
 translate([0,0,0])cube([16.2,15.2,10],center=true);
 translate([18,0,0])cube([16.2,15.2,10],center=true);
 translate([0,18,0])cube([16.2,15.2,10],center=true);
 translate([-18,0,0])cube([16.2,15.2,10],center=true);
 translate([0,-18,0])cube([16.2,15.2,10],center=true);
  
 translate([-20,-20,0])cube([20,20,10],center=true);
  translate([-20,20,0])cube([20,20,10],center=true);
  translate([20,-20,0])cube([20,20,10],center=true);
  translate([20,20,0])cube([20,20,10],center=true);

 }

}

keySpacer();