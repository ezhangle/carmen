var Benchmark = require('benchmark');
var suite = new Benchmark.Suite();
var unidecode = require('unidecode');
var token = require('../lib/util/token');

var tokens = [
    { from: "First", to: "1st" },
    { from: "Second", to: "2nd" },
    { from: "Third", to: "3rd" },
    { from: "Fourth", to: "4th" },
    { from: "Fifth", to: "5th" },
    { from: "Sixth", to: "6th" },
    { from: "Seventh", to: "7th" },
    { from: "Eigth", to: "8th" },
    { from: "Ninth", to: "9th" },
    { from: "Tenth", to: "10th" },
    { from: "Eleventh", to: "11th" },
    { from: "Twelfth", to: "12th" },
    { from: "Thirteenth", to: "13th" },
    { from: "Fourteenth", to: "14th" },
    { from: "Fifteenth", to: "15th" },
    { from: "Sixteenth", to: "16th" },
    { from: "Seventeenth", to: "17th" },
    { from: "Eighteenth", to: "18th" },
    { from: "Nineteenth", to: "19th" },
    { from: "Twentieth", to: "20th" },
    { from: "Alley", to: "Aly" },
    { from: "Arcade", to: "Arc" },
    { from: "Avenue", to: "Ave" },
    { from: "Bayou", to: "Byu" },
    { from: "Beach", to: "Bch" },
    { from: "Bluff", to: "Blf" },
    { from: "Bottom", to: "Btm" },
    { from: "Boulevard", to: "Blvd" },
    { from: "Branch", to: "Br" },
    { from: "Bridge", to: "Brg" },
    { from: "Brook", to: "Brk" },
    { from: "Brooks", to: "Brks" },
    { from: "Burg", to: "Bg" },
    { from: "Burgs", to: "Bgs" },
    { from: "Bypass", to: "Byp" },
    { from: "Calle", to: "Cll" },
    { from: "Camp", to: "Cp" },
    { from: "Canyon", to: "Cyn" },
    { from: "Cape", to: "Cpe" },
    { from: "Causeway", to: "Cswy" },
    { from: "Center", to: "Ctr" },
    { from: "Centers", to: "Ctrs" },
    { from: "Circle", to: "Cir" },
    { from: "Circles", to: "Cirs" },
    { from: "Cliff", to: "Clf" },
    { from: "Cliffs", to: "Clfs" },
    { from: "Club", to: "Clb" },
    { from: "Common", to: "Cmn" },
    { from: "Corner", to: "Cor" },
    { from: "Course", to: "Crse" },
    { from: "Court", to: "Ct" },
    { from: "Courts", to: "Cts" },
    { from: "Cove", to: "Cv" },
    { from: "Creek", to: "Crk" },
    { from: "Crescent", to: "Cres" },
    { from: "Crest", to: "Crst" },
    { from: "Crossing", to: "Xing" },
    { from: "Curve", to: "Curv" },
    { from: "Dale", to: "Dl" },
    { from: "Dam", to: "Dm" },
    { from: "Divide", to: "Dv" },
    { from: "Drive", to: "Dr" },
    { from: "Drives", to: "Drs" },
    { from: "East", to: "E" },
    { from: "Estate", to: "Est" },
    { from: "Estates", to: "Ests" },
    { from: "Expressway", to: "Expy" },
    { from: "Extension", to: "Ext" },
    { from: "Extensions", to: "Exts" },
    { from: "Falls", to: "Fls" },
    { from: "Ferry", to: "Fry" },
    { from: "Field", to: "Fld" },
    { from: "Fields", to: "Flds" },
    { from: "Flat", to: "Flt" },
    { from: "Flats", to: "Flts" },
    { from: "Ford", to: "Frd" },
    { from: "Forest", to: "Frst" },
    { from: "Forge", to: "Frg" },
    { from: "Forges", to: "Frgs" },
    { from: "Fork", to: "Frk" },
    { from: "Fort", to: "Ft" },
    { from: "Freeway", to: "Fwy" },
    { from: "Grade", to: "Grd" },
    { from: "Green", to: "Grn" },
    { from: "Harbor", to: "Hbr" },
    { from: "Harbors", to: "Hbrs" },
    { from: "Haven", to: "Hvn" },
    { from: "Heights", to: "Hts" },
    { from: "Highway", to: "Hwy" },
    { from: "Hill", to: "Hl" },
    { from: "Hills", to: "Hls" },
    { from: "Hollow", to: "Holw" },
    { from: "Industrial", to: "Ind" },
    { from: "Interstate", to: "I" },
    { from: "Island", to: "Is" },
    { from: "Islands", to: "Iss" },
    { from: "Junction", to: "Jct" },
    { from: "Junctions", to: "Jcts" },
    { from: "Junior", to: "Jr" },
    { from: "Key", to: "Ky" },
    { from: "Keys", to: "Kys" },
    { from: "Knoll", to: "Knl" },
    { from: "Knolls", to: "Knls" },
    { from: "Lake", to: "Lk" },
    { from: "Lakes", to: "Lks" },
    { from: "Landing", to: "Lndg" },
    { from: "Lane", to: "Ln" },
    { from: "Lieutenant", to: "Lt" },
    { from: "Light", to: "Lgt" },
    { from: "Lights", to: "Lgts" },
    { from: "Loaf", to: "Lf" },
    { from: "Lock", to: "Lck" },
    { from: "Locks", to: "Lcks" },
    { from: "Lodge", to: "Ldg" },
    { from: "Mall", to: "Mal" },
    { from: "Manor", to: "Mnr" },
    { from: "Manors", to: "Mnrs" },
    { from: "Meadow", to: "Mdw" },
    { from: "Meadows", to: "Mdws" },
    { from: "Mill", to: "Ml" },
    { from: "Mission", to: "Msn" },
    { from: "Moorhead", to: "Mhd" },
    { from: "Motorway", to: "Mtwy" },
    { from: "Mountain", to: "Mtn" },
    { from: "Mount", to: "Mt" },
    { from: "Neck", to: "Nck" },
    { from: "Northeast", to: "NE" },
    { from: "North", to: "N" },
    { from: "Northwest", to: "NW" },
    { from: "Orchard", to: "Orch" },
    { from: "Overpass", to: "Ovps" },
    { from: "Parkway", to: "Pkwy" },
    { from: "Passage", to: "Psge" },
    { from: "Place", to: "Pl" },
    { from: "Plain", to: "Pln" },
    { from: "Plains", to: "Plns" },
    { from: "Plaza", to: "Plz" },
    { from: "Point", to: "Pt" },
    { from: "Points", to: "Pts" },
    { from: "Port", to: "Prt" },
    { from: "Ports", to: "Prts" },
    { from: "Prairie", to: "Pr" },
    { from: "Private", to: "Pvt" },
    { from: "Radial", to: "Radl" },
    { from: "Ranch", to: "Rnch" },
    { from: "Rapid", to: "Rpd" },
    { from: "Rapids", to: "Rpds" },
    { from: "Rest", to: "Rst" },
    { from: "Ridge", to: "Rdg" },
    { from: "Ridges", to: "Rdgs" },
    { from: "River", to: "Riv" },
    { from: "Road", to: "Rd" },
    { from: "Roads", to: "Rds" },
    { from: "Route", to: "Rte" },
    { from: "Saint", to: "St" },
    { from: "Senior", to: "Sr" },
    { from: "Sergeant", to: "Sgt" },
    { from: "Shoal", to: "Shl" },
    { from: "Shoals", to: "Shls" },
    { from: "Shore", to: "Shr" },
    { from: "Shores", to: "Shrs" },
    { from: "Skyway", to: "Skwy" },
    { from: "Southeast", to: "SE" },
    { from: "South", to: "S" },
    { from: "Southwest", to: "SW" },
    { from: "Spring", to: "Spg" },
    { from: "Springs", to: "Spgs" },
    { from: "Square", to: "Sq" },
    { from: "Squares", to: "Sqs" },
    { from: "Station", to: "Sta" },
    { from: "Stream", to: "Strm" },
    { from: "Streets", to: "Sts" },
    { from: "Street", to: "St" },
    { from: "Summit", to: "Smt" },
    { from: "Terrace", to: "Ter" },
    { from: "Thoroughfare", to: "Thfr" },
    { from: "Thruway", to: "Thwy" },
    { from: "Trace", to: "Trce" },
    { from: "Trafficway", to: "Tfwy" },
    { from: "Trail", to: "Trl" },
    { from: "Tunnel", to: "Tunl" },
    { from: "Turnpike", to: "Tpke" },
    { from: "Underpass", to: "Unp" },
    { from: "Unions", to: "Uns" },
    { from: "Union", to: "Un" },
    { from: "Valleys", to: "Vlys" },
    { from: "Valley", to: "Vly" },
    { from: "Viaduct", to: "Via" },
    { from: "Views", to: "Vws" },
    { from: "View", to: "Vw" },
    { from: "Villages", to: "Vlgs" },
    { from: "Village", to: "Vlg" },
    { from: "Ville", to: "Vl" },
    { from: "Vista", to: "Vis" },
    { from: "Walkway", to: "Wkwy" },
    { from: "West", to: "W" } ];

var replacers = token.createReplacer(tokens);

suite.add('token replace', function() {
    token.replaceToken(replacers, 'kanye west');
})
.on('cycle', function(event) {
    console.log(String(event.target));
})
.run();
