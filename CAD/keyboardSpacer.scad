module keySpacer(){
 
 difference(){
 translate([-1.75,0,-1.5])cube([21.6, 90/2-3, 4.8+1.5+0.2+1]);
 translate([0,0,-1.5])cube([50, 90/2, 4.8+1.5+0.2+1]);
 }
 difference(){
 cube([ 25,90/2-3, 6]);
 
 translate([12.1,15/2+3,0])cube([16,15,100],center=true);
 translate([12.1,15/2+23,0])cube([16,15,100],center=true);

 }

}

keySpacer();