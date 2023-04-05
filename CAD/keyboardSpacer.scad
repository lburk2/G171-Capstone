module keySpacer(){
 
 difference(){
 translate([-1.75,0,-1.5])cube([21.6, 90/2, 4.8+1.5]);
 translate([0,0,-1.5])cube([50, 90/2, 4.8+1.5]);
 }
 difference(){
 cube([ 21.6,90/2, 4.8]);
 
 translate([20,10.8,0])cube([13.5,13.7,10],center=true);
 translate([20,32.4,0])cube([13.5,13.7,10],center=true);

 }

}

keySpacer();