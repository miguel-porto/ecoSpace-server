library(maptools)
w=readShapePoly("/media/miguel/Brutal/FotosAereas/Cartas varias/World1.shp")
coo=lapply(w@polygons,function(x) {a=x@Polygons[[1]]@coords;a[,2]=-a[,2];return(a)})
paths=lapply(coo,function(x) {
	ini=paste("M",paste(round(x[1,],4),collapse=" "),sep="")
	paste(ini,paste(paste("L",apply(round(x,3),1,paste,collapse=" "),sep=""),collapse=""),sep="")
})

#write("<svg width=\"1000\" height=\"500\" viewBox=\"-180 -90 360 180\">",file="worldpaths.svg")
write(paste("<path d=\"",paths,"Z\"/>",sep=""),file="worldpaths.svg",append=T)
#write("</svg>",file="worldpaths.svg",append=T)

