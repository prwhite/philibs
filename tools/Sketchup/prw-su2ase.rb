# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place - Suite 330, Boston, MA 02111-1307, USA, or go to
# http://www.gnu.org/copyleft/lesser.txt.
#-----------------------------------------------------------------------------
# Name        	: PRWSU2ASE.rb
# Description 	: Export models to 3DS Max ASCII format
# Menu Item   	: Plugins\ASE Exporter\Export
# Author      	: Belgabor
#		  Code used form following SU exporters: SU2KT by Tomasz Marek, SU2POV by Didier Bur and OGRE exporter by Kojack
# Usage     	: Copy script to PLUGINS folder in SketchUp folder, run SU, go to Plugins\ASE exporter
# Note	: This script is written to export ASE for importing custom scenery into Atari's RollerCoaster Tycoon 3, so there are lots of features missing.
#		  (And other seemingly strange ones present :p)
# Date        	: 05.02.2007
# Type        	: Exporter
# Version     	: 0.4 Wrong UV coordinates fixed, export text as effect point on SketchUp 6
#		  0.3 Feedback on exported meshes and face count.
#		  0.2 Interaction with rct3.rb for effect points
#		  0.1 Initial release
#-----------------------------------------------------------------------------

require 'sketchup.rb'

class PRWSU2ASE

# ---- Settings and global variables ----- #

	def PRWSU2ASE::reset_global_variables
	
    $g_prwsu2ase_scale = 1.0
		$g_prwsu2ase_export_constructionpoints = false
		$g_prwsu2ase_model_name=""
		$g_prwsu2ase_count_tri = 0
		$g_prwsu2ase_count_material = 0
    $g_prwsu2ase_mat2id = {}
		$g_prwsu2ase_exported_meshes = {}
    $g_prwsu2ase_indent_level = 0
    $g_prwsu2ase_gen_name_val = 0
    $g_prwsu2ase_out = nil
		$g_prwsu2ase_parent_mat=[]
    $g_prwsu2ase_active_materials = {}
		# $current_camera=Sketchup.active_model.active_view.camera

	
	end


##### ------  Exporting starts here ------ ######

def PRWSU2ASE::prefs
  results = inputbox( 
      ["Export Selected", "Diffuse = white", "Texture ext = ", "Export Hidden", "Export All Materials", "Use Single Selection Name for File" ], 
      ["Yes", "Yes", "dds", "No", "No", "Yes" ], 
      ["Yes|No", "Yes|No", "(orig)|dds", "Yes|No", "Yes|No", "Yes|No" ], 
      "ASE Export Preferences" )
      
  if not results
    return
  end

  # $g_prwsu2ase_selected=true
  # $g_prwsu2ase_tex_ext = "dds"                 # TODO: add pref
  # $g_prwsu2ase_mat_diffuse = Sketchup::Color.new(255, 255, 255)   # TODO: add pref
  
  if results[ 0 ] == "Yes" then
    $g_prwsu2ase_export_selected=true
  else
    $g_prwsu2ase_export_selected=false
  end

  if results[ 1 ] == "Yes" then
    $g_prwsu2ase_mat_diffuse = Sketchup::Color.new(255, 255, 255)
  else
    $g_prwsu2ase_mat_diffuse = nil
  end

  if results[ 3 ] == "Yes" then
    $g_prwsu2ase_write_hidden = true
  else
    $g_prwsu2ase_write_hidden = false
  end

  if results[ 4 ] == "Yes" then
    $g_prwsu2ase_write_all_materials = true
  else
    $g_prwsu2ase_write_all_materials = false
  end
  
  # $g_prwsu2ase_write_solo_object_name
  if results[ 5 ] == "Yes" then
    $g_prwsu2ase_write_solo_object_name = true
  else
    $g_prwsu2ase_write_solo_object_name = false
  end
  
  $g_prwsu2ase_tex_ext = results[ 2 ]
end

#####################################################################

def PRWSU2ASE::export
	
	PRWSU2ASE.reset_global_variables

    # prepare save dialog
  if Sketchup.active_model.selection.length > 0	&& $g_prwsu2ase_export_selected == true then
    $g_prwsu2ase_selected=true 
  else
    $g_prwsu2ase_selected=false
  end
	export_text="Export Model to ASE"
	export_text="Export SELECTION to ASE" if $g_prwsu2ase_selected==true

	model = Sketchup.active_model
	pages = model.pages	
		
	model_filename = File.basename(model.path)
	
  if model.selection.length == 1 && $g_prwsu2ase_export_selected && $g_prwsu2ase_write_solo_object_name then
    model_name = model.selection[ 0 ].name
  else
  	if model_filename!=""
  		model_name = model_filename.split(".")[0]
  		model_name += ".ase"
  	else
  		model_name = "Untitled.ase" 	
  	end
  end
  
	
	$g_prwsu2ase_export_file=UI.savepanel(export_text, "" , model_name)

	return if $g_prwsu2ase_export_file==nil		

	if $g_prwsu2ase_export_file==$g_prwsu2ase_export_file.split(".")[0]
		$g_prwsu2ase_export_file+=".ase"
	end
	$g_prwsu2ase_model_name=File.basename($g_prwsu2ase_export_file)
	$g_prwsu2ase_model_name=$g_prwsu2ase_model_name.split(".")[0]

	if $g_prwsu2ase_model_name!=""	
		out = File.new($g_prwsu2ase_export_file,"w") 
    $g_prwsu2ase_out = out

		$g_prwsu2ase_path_textures=File.dirname($g_prwsu2ase_export_file)

		start_time=Time.new

      # here's where the real work happens
    
		PRWSU2ASE.write_header(out)
    
    PRWSU2ASE.write_materials(model,out)
		
		PRWSU2ASE.export_mesh(out)
		
		# PRWSU2ASE.export_constructionpoints(out)
		
		out.close
    $g_prwsu2ase_out = nil

      # now finish up with more dialog/status bullshit.
		end_time=Time.new
		elapsed=end_time-start_time
		time="Model exported in "
			(time=time+"#{(elapsed/3600).floor}h ";elapsed-=(elapsed/3600).floor*3600) if (elapsed/3600).floor>0
			(time=time+"#{(elapsed/60).floor}m ";elapsed-=(elapsed/60).floor*60) if (elapsed/60).floor>0
			time=time+"#{elapsed.round}s. "

		PRWSU2ASE.status_bar(time+" Triangles = #{$g_prwsu2ase_count_tri}")
		export_text="Model saved in file:\n"
		export_text="Selection saved in file:\n" if $g_prwsu2ase_selected==true
		export_text=export_text + $g_prwsu2ase_export_file
		if ($g_prwsu2ase_exported_meshes != {})
			export_text = export_text + "\n\nExported Meshes:\n"
			warn = false
			for (name,faces) in $g_prwsu2ase_exported_meshes
				export_text = export_text + name
				export_text = export_text + " (#{faces} faces)" if faces>0
				export_text = export_text + "\n"
			end
      export_text += "\nMaterial Count: #{$g_prwsu2ase_count_material}\n"
		end
	 UI.messagebox export_text

	end

	PRWSU2ASE.reset_global_variables

end

# ----------------------------------- About message
def PRWSU2ASE::about
	UI.messagebox("prw_su2ase.rb version 0.5.  2009/01/20.

Purpose: Export SketchUp model to 3DS Max ASCII scene export format.

Author: Payton R. White (payton@prehiti.net)

Note: This exporter enhances the previous work by exporting a full model hierarchy and enhanced material information.  It also changes the preferences to be persistent for a single session.


Previous About (some of which is no longer accurate):

SU2ASE version 0.4. 5th February 2007\nFreeware SketchUp Exporter to 3DS Max ASCII\nAuthor: Belgabor\n\nBased on Su2Kt by Tomasz Marek,\nSu2Pov written by Didier Bur\nand OGRE exporter by Kojack\n
NOTE: This script is written to export ASE for importing custom scenery into Atari's RollerCoaster Tycoon 3, so there are lots of features missing.\n
USAGE:
* Use groups or component instances for \"meshes\" in the RCT3 Importer.
* Component instances have two names, the component name and the instance name. The latter is used.
* You need to map a bitmap texture, basic colours will NOT work.
* Construction points can be used as single vertex meshes for effect points.
* On SketchUp 6 it is also possible to use text for single vertex meshes for effect points." , MB_MULTILINE , "SU2ASE - Model Exporter to ASE") 
end

#### -------------- Export rendering settings ----------------- ##########

def PRWSU2ASE::write_header(out)

	out.puts "*3DSMAX_ASCIIEXPORT	200"
	out.puts "*COMMENT \"AsciiExport Version  2.00 - Exported from SketchUp by su2ase\""
	out.puts "*SCENE {"
	out.puts "	*SCENE_FILENAME \""+$g_prwsu2ase_export_file+"\""
	out.puts "	*SCENE_FIRSTFRAME 1"
	out.puts "	*SCENE_LASTFRAME 250"
	out.puts "	*SCENE_FRAMESPEED 25"
	out.puts "	*SCENE_TICKSPERFRAME 160"
	out.puts "	*SCENE_BACKGROUND_STATIC 0.0566 0.2208 0.4000"
	out.puts "	*SCENE_AMBIENT_STATIC 0.0000 0.0000 0.0000"
	out.puts "}"

end

################################################################################

def PRWSU2ASE::fixupTextureName( fname )
  if $g_prwsu2ase_tex_ext != "(orig)" then
    ret = fname.scan( /(.*)\.(.*)/)

    root = ret[ 0 ][ 0 ]
    ext = ret[ 0 ][ 1 ]

    if ext != nil && ext != "" then
      fname = "#{root}.#{$g_prwsu2ase_tex_ext}"
    end
  end
    
  fname
end

def PRWSU2ASE::write_materials(model,out)

  collectActiveMaterials

	out.puts "*MATERIAL_LIST {"

    # TODO: Iterate over materials.
    # TODO: Put all materials in a dictionary so we can
    #   lookup their integer IDs during mesh writing.
	#out.puts "	*MATERIAL_COUNT 0"

  materials = $g_prwsu2ase_active_materials.keys
  num = materials.length
  $g_prwsu2ase_count_material = num
  matid = 0
  
  out.puts "  *MATERIAL_COUNT #{num}"

#  materials.each { |mat|
  for mat in materials

    $g_prwsu2ase_mat2id[mat] = matid
  
    if $g_prwsu2ase_mat_diffuse == nil || mat.texture == nil then
      color = mat.color
    else
      color = $g_prwsu2ase_mat_diffuse
    end

    emiss = 0.0000
    alpha = 1.0 - mat.alpha
    
    r = color.red / 255.0
    g = color.green / 255.0
    b = color.blue / 255.0
    # a = color.alpha / 255.0
    
    out.puts "  *MATERIAL #{matid} {"
		out.puts "    *MATERIAL_NAME \"#{mat.display_name}\""
		out.puts "    *MATERIAL_CLASS \"Standard\""
		out.puts "    *MATERIAL_AMBIENT 1.0 1.0 1.0"
		out.puts "    *MATERIAL_DIFFUSE #{r} #{g} #{b}"
		out.puts "    *MATERIAL_SPECULAR 1.0000 1.0000 1.0000 1.0000"
		out.puts "    *MATERIAL_SHINE 0.1000"
		out.puts "    *MATERIAL_SHINESTRENGTH 0.0000"
		out.puts "    *MATERIAL_TRANSPARENCY #{alpha}"
		out.puts "    *MATERIAL_WIRESIZE 1.0000"
		out.puts "    *MATERIAL_SHADING Blinn"
		out.puts "    *MATERIAL_XP_FALLOFF 0.0000"
		out.puts "    *MATERIAL_SELFILLUM #{emiss}"
		out.puts "    *MATERIAL_FALLOFF In"
		out.puts "    *MATERIAL_XP_TYPE Filter"
    
    if mat.texture != nil then
      tex = mat.texture
      
      fname = fixupTextureName( tex.filename )
    
      out.puts "    *MAP_DIFFUSE {"
			out.puts "      *MAP_NAME \"\""
			out.puts "      *MAP_CLASS \"Bitmap\""
			out.puts "      *MAP_SUBNO 1"
			out.puts "      *MAP_AMOUNT 1.0000"
			out.puts "      *BITMAP \"#{fname}\""
			out.puts "      *MAP_TYPE Screen"
			out.puts "      *UVW_U_OFFSET 0.0000"
			out.puts "      *UVW_V_OFFSET 0.0000"
			out.puts "      *UVW_U_TILING 1.0000"
			out.puts "      *UVW_V_TILING 1.0000"
			out.puts "      *UVW_ANGLE 0.0000"
			out.puts "      *UVW_BLUR 1.0000"
			out.puts "      *UVW_BLUR_OFFSET 0.0000"
			out.puts "      *UVW_NOUSE_AMT 1.0000"
			out.puts "      *UVW_NOISE_SIZE 1.0000"
			out.puts "      *UVW_NOISE_LEVEL 1"
			out.puts "      *UVW_NOISE_PHASE 0.0000"
			out.puts "      *BITMAP_FILTER Pyramidal"
      out.puts "    }"

    end

    out.puts "  }"
    
    matid += 1
#	}
  end
  
  out.puts "}"

end

################################################################################
#### - Send text to status bar - ####
def PRWSU2ASE::status_bar(stat_text)
	
	statbar = Sketchup.set_status_text stat_text
	
end

################################################################################
#### -------- Mesh export ---------- ######

def PRWSU2ASE::export_mesh(export_name)
	
	$g_prwsu2ase_parent_mat=[]
		
	if $g_prwsu2ase_selected==true
		ents = []
		Sketchup.active_model.selection.each{|e| ents = ents + [e]}
		PRWSU2ASE.export_entities( export_name, ents, Geom::Transformation.new, nil)
	else
		PRWSU2ASE.export_entities( export_name, Sketchup.active_model.entities, Geom::Transformation.new, nil)
	end
end

################################################################################
################################################################################
################################################################################

def PRWSU2ASE::collectActiveMaterials()

  if $g_prwsu2ase_write_all_materials then
    mats = Sketchup.active_model.materials
    for mat in mats
      $g_prwsu2ase_active_materials[ mat ] = mat
    end
  
    $g_prwsu2ase_active_materials={}
    if $g_prwsu2ase_selected==true
      ents = []
      Sketchup.active_model.selection.each{|e| ents = ents + [e]}
      PRWSU2ASE.collectActiveMaterialsFromEntities( ents )
    else
      PRWSU2ASE.collectActiveMaterialsFromEntities( Sketchup.active_model.entities )
    end
  end
end

################################################################################

def PRWSU2ASE::collectActiveMaterialsFromEntities( ents )

  faces = collectType(ents, Sketchup::Face)
  groups = collectType(ents, Sketchup::Group)

  if faces != [] then
    for face in faces
      mat = face.material
      if mat != nil then
#puts "adding face material"
        $g_prwsu2ase_active_materials[ mat ] = mat
      end
    end
  end
  
  if groups != [] then
    for group in groups
      mat = group.material
      if mat != nil then
puts "adding group material"
        $g_prwsu2ase_active_materials[ mat ] = mat
      end
      
      if group.entities != [] then
        collectActiveMaterialsFromEntities group.entities
      end
    end
  end
    
end


################################################################################
# i/o routines.

def PRWSU2ASE::indent
  out = ""
  count = 0
  
  while count < $g_prwsu2ase_indent_level do
    out += "  "
    count += 1
  end
  
  out
end

def PRWSU2ASE::incrIndent(val = 1)
  $g_prwsu2ase_indent_level += val
end

def PRWSU2ASE::puts(what)
  $g_prwsu2ase_out.puts indent+what
end

def PRWSU2ASE::print(what)
  $g_prwsu2ase_out.print indent+what
end

#####################################################################################
#####################################################################################
#####################################################################################
# collect a specific type of objects from an array.

def PRWSU2ASE::collectType(entities, type)
  retVal = []
  
  for ent in entities
    if ent.class == type then
      retVal = retVal + [ent]
    end
  end

  retVal
end

#####################################################################################
#####################################################################################
#####################################################################################
# manage material stack.

def PRWSU2ASE::pushMaterial( mat )
  if mat != nil then
    $g_prwsu2ase_parent_mat.push(mat)
    # PRWSU2ASE.store_textured_entities(e)			
  else
    $g_prwsu2ase_parent_mat.push($g_prwsu2ase_parent_mat.last)
  end
end

def PRWSU2ASE::popMaterial
  $g_prwsu2ase_parent_mat.pop
end

def PRWSU2ASE::getCurrentMaterial
  if $g_prwsu2ase_parent_mat.last == nil then
    return 0  # not really the best idea... we should maybe write
              # out an approximation of the sketchup def material.
  else
    return $g_prwsu2ase_parent_mat.last
  end
end

#####################################################################################
#####################################################################################
#####################################################################################

  # generate a unique name for any entities that don't have a name due
  # to being at the root or when geometry is a child of a group with
  # group siblings.
def PRWSU2ASE::genName( base )
  $g_prwsu2ase_gen_name_val += 1
  "#{base}-#{$g_prwsu2ase_gen_name_val}"
end

###################

def PRWSU2ASE::write_group( out, object, trans, parent )
  if object.hidden? && ! $g_prwsu2ase_write_hidden then
    return
  end

  faces = collectType(object.entities, Sketchup::Face)
  groups = collectType(object.entities, Sketchup::Group)
  facesByMat = collectFacesByMaterial( faces )
  
  pushMaterial object.material

  newTrans = trans * object.transformation
  doGroup = false

#puts( "write_group newTrans for #{object.name} = " )
#putsXform( newTrans )
  
  if groups != [] then
    doGroup = true
  elsif faces == [] then
    doGroup = true
  elsif facesByMat.size != 1
    doGroup = true
  end

  if doGroup then
    out.puts "#{indent()}*GROUP \"#{object.name}\" {"
    
    incrIndent
    
  end

  export_entities( out, groups, newTrans, object )
  
  facesByMat.each_key { |mat|
    if doGroup then
      name = genName( object.name )
    else
        # this is meant to cascade through... it's a reverse order
        # precedence thingie.
      name = genName( object.class() )
      if parent != nil && parent.name != "" then
        name = genName( parent.name )
      end
      if object.name != "" then
        name = object.name
      end
    end
  
    # write_faces( out, faces, newTrans, object, name )
    write_faces( out, facesByMat[ mat ], newTrans, object, name )
  }
  
  if doGroup then
    incrIndent -1
  
    out.puts "#{indent()}}"
  end

  popMaterial
end

#####################################################################################
#####################################################################################
#####################################################################################
  # return a hash of material/face arrays.
  
def PRWSU2ASE::collectFacesByMaterial( faces )
  facesByMat = {}
  
  for face in faces
    mat = face.material
    
    if face.hidden? && ! $g_prwsu2ase_write_hidden then
      continue
    end
    
    if facesByMat[ mat ] == nil then
      facesByMat[ mat ] = [face]
    else
      facesByMat[ mat ] += [face]
    end
  end

  facesByMat
end

#####################################################################################

def PRWSU2ASE::write_faces( out, faces, trans, parent, name )
  pushMaterial faces[ 0 ].material
  
  do_write_faces( out, faces, trans, parent, name )
  
  popMaterial
end

#####################################################################################
#####################################################################################
#####################################################################################

def PRWSU2ASE::export_entities( out, elist, trans, parent )  

#puts( "export_entities" )
#putsXform( trans )

  faces = collectType(elist, Sketchup::Face)
  groups = collectType(elist, Sketchup::Group)
  
  if faces != [] then
    name = genName( "root" )
    write_faces( out, faces, trans, parent, name )
  end
  
  if groups != [] then
    for group in groups
      write_group( out, group, trans, parent )
    end
  end
end

#####################################################################################
#####################################################################################
#####################################################################################

def PRWSU2ASE::point_to_vector(p)
	Geom::Vector3d.new(p.x,p.y,p.z)
end

#####################################################################################
#####################################################################################
#####################################################################################

def PRWSU2ASE::write_tmat(out,trans,parent,name)

  pts = trans.to_a
  orig = trans.origin
  
puts "	*NODE_TM {"
puts "		*NODE_NAME \"#{name}\""
puts "		*INHERIT_POS 0 0 0"
puts "		*INHERIT_ROT 0 0 0"
puts "		*INHERIT_SCL 0 0 0"

puts "		*TM_ROW0 #{"%.4f" %(pts[0])} #{"%.4f" %(pts[1])} #{"%.4f" %(pts[2])}"
puts "		*TM_ROW1 #{"%.4f" %(pts[4])} #{"%.4f" %(pts[5])} #{"%.4f" %(pts[6])}"
puts "		*TM_ROW2 #{"%.4f" %(pts[8])} #{"%.4f" %(pts[9])} #{"%.4f" %(pts[10])}"
puts "		*TM_ROW3 #{"%.4f" %(pts[12])} #{"%.4f" %(pts[13])} #{"%.4f" %(pts[14])}"
# puts "		*TM_ROW3 #{"%.4f" %(orig[0])} #{"%.4f" %(orig[1])} #{"%.4f" %(orig[2])}"

puts "		*TM_POS 1.0000 1.0000 0.0000"
puts "		*TM_ROTAXIS 1.0000 0.0000 0.0000"
puts "		*TM_ROTANGLE 0.0000"
puts "		*TM_SCALE 1.0000 1.0000 1.0000"
puts "		*TM_SCALEAXIS 1.0000 0.0000 0.0000"
puts "		*TM_SCALEAXISANG 0.0000"
puts "	}"

end

############################################################################
############################################################################
############################################################################

  # This is pretty heavy.  It uses maps to figure out which 
  # vertices/uvs are shared, and then outputs a hash of
  # unshared ids to shared ids and a map of shared ids to values.
  # Sharing verts looks to reduce typical file sizes ~10-20%.
def PRWSU2ASE::genAttrMap( meshes, deref )
  map = {}
  pt2Ind = {}     # map of point strings to shared indices
  ind2Pt = {}     # map of non-shared indices to points
  ind2PtOut = {}  # map of shared indices to points
  count = 0
  cur = 0
  
  for mesh in meshes
    for ind in ( 1..mesh.count_points )
      # pt = mesh.point_at( ind )
      pt = deref.call( mesh, ind )
      ptStr = pt.to_s
      if pt2Ind[ ptStr ] == nil
        pt2Ind[ ptStr ] = cur
        ind2PtOut[ cur ] = pt
        cur += 1
      end
      ind2Pt[ count ] = pt
      count += 1
    end
  end
  
  for ind in ( 0..ind2Pt.size - 1 )
    pt = ind2Pt[ ind ]
    outInd = pt2Ind[ pt.to_s ]
# puts "#{ind} = #{outInd}"
    map[ ind ] = outInd
  end
  
  # mmap = {}
  # for ind in ( 0..pt2Ind.size - 1 )
    
  # end
  
  [map,ind2PtOut]
end

############################################################################
############################################################################
############################################################################
##### ---- Exporting polymeshes ---- #######

def PRWSU2ASE::putsXform( xform )
  arr = xform.to_a
  for yy in ( 0..3 )
    puts "#{arr[yy*4+0]} #{arr[yy*4+1]} #{arr[yy*4+2]} #{arr[yy*4+3]}"
  end
end

def PRWSU2ASE::do_write_faces( out, faces, trans, parent, name )
	meshes = []     # format was [geom::PolygonMesh, material]
	polycount = 0
	pointcount = 0
  
	mirrored={}
  twoSided=[]

#puts( "do_write_faces for #{name}" )
#putsXform( trans )
  
    # ft is [ face, trans, material ]
	for face in faces
		polymesh = face.mesh 5
		polymesh.transform! trans
		mirrored[polymesh] = true
		xa = PRWSU2ASE.point_to_vector(trans.xaxis)
		ya = PRWSU2ASE.point_to_vector(trans.yaxis)
		za = PRWSU2ASE.point_to_vector(trans.zaxis)
		xy = xa.cross(ya)
		xz = xa.cross(za)
		yz = ya.cross(za)
		if xy.dot(za) < 0
			mirrored[polymesh] = !mirrored[polymesh]
		end
		if xz.dot(ya) < 0
			mirrored[polymesh] = !mirrored[polymesh]
		end
		if yz.dot(xa) < 0
			mirrored[polymesh] = !mirrored[polymesh]
		end

		meshes = meshes + [polymesh]

		polycount=polycount + polymesh.count_polygons
		pointcount=pointcount + polymesh.count_points
    
    if ! face.casts_shadows? then
      # This is painful... face.dup and face.clone don't work in
      # SU... (dup and clone only work for a few things like components).
      # Thus, we take and existing face, reverse it, get a [new] mesh
      # out of it and then reverse it back so the model in the editor
      # isn't hosed.  Hopefully this doesn't add creeping FP error
      # over time.  Note face.reverse! does not creat a new face.
      rface = face.reverse!
      tmpMesh = rface.mesh 5
      rface = face.reverse!
      tmpMesh.transform! trans
      mirrored[ tmpMesh ] = mirrored[ polymesh ]
      meshes = meshes + [ tmpMesh ]
      twoSided = twoSided + [ tmpMesh ]

  		polycount=polycount + tmpMesh.count_polygons
  		pointcount=pointcount + tmpMesh.count_points      
    end
	end

    # leave the legacy code working with new way of tracking name
  grname = name
	
	$g_prwsu2ase_exported_meshes[grname] = polycount


puts "*GEOMOBJECT {"
puts "	*NODE_NAME \"#{grname}\""

  PRWSU2ASE.write_tmat(out,trans,parent,name)

  oldway = false
  
  if oldway
    vertices = pointcount
  else
    maps = genAttrMap( meshes, Proc.new { |mesh, ind| mesh.point_at( ind ) } )
    vmap = maps[ 0 ]
    mmap = maps[ 1 ]
    vertices = mmap.size
  end

puts "	*MESH {"
puts "		*TIMEVALUE 0"
puts "		*MESH_NUMVERTEX #{vertices}"
puts "		*MESH_NUMFACES #{polycount}"

#Exporting vertices

PRWSU2ASE.status_bar("Group being exported: " + grname + " - Vertices")

puts "		*MESH_VERTEX_LIST {"

  if oldway then
  	po = 0
  	for mesh in meshes
  		for p in (1..mesh.count_points)
  			pos = mesh.point_at(p).to_a
  			puts "			*MESH_VERTEX #{po} #{"%.4f" %(pos[0]*$g_prwsu2ase_scale)} #{"%.4f" %(pos[1]*$g_prwsu2ase_scale)} #{"%.4f" %(pos[2]*$g_prwsu2ase_scale)}"		
  			po = po + 1
  		end
    end
  else
    for ind in ( 0..mmap.length - 1 )
      pos = mmap[ ind ]
      puts "			*MESH_VERTEX #{ind} #{"%.4f" %(pos[0]*$g_prwsu2ase_scale)} #{"%.4f" %(pos[1]*$g_prwsu2ase_scale)} #{"%.4f" %(pos[2]*$g_prwsu2ase_scale)}"		
    end
  end
    
puts "		}"

#Exporting faces

PRWSU2ASE.status_bar("Group being exported: " + grname + " - Faces")
puts "		*MESH_FACE_LIST {"
	
	startindex = 0
	po = 0
	for mesh in meshes
		for poly in mesh.polygons
			v1 = (poly[0]>=0?poly[0]:-poly[0])+startindex
			v2 = (poly[1]>=0?poly[1]:-poly[1])+startindex
			v3 = (poly[2]>=0?poly[2]:-poly[2])+startindex

      v1 -= 1
      v2 -= 1
      v3 -= 1
        
      if ! oldway
        v1 = vmap[ v1 ]
        v2 = vmap[ v2 ]
        v3 = vmap[ v3 ]
      end
        
			if !mirrored[mesh]
				puts "			*MESH_FACE   #{po}: A:     #{v1} B:     #{v2} C:     #{v3} AB: 1 BC: 0 CA: 1 *MESH_SMOOTHING 0 *MESH_MTLID 0"
			else
				puts "			*MESH_FACE   #{po}: A:     #{v2} B:     #{v1} C:     #{v3} AB: 1 BC: 0 CA: 1 *MESH_SMOOTHING 0 *MESH_MTLID 0"
			end
			$g_prwsu2ase_count_tri = $g_prwsu2ase_count_tri + 1
			po = po + 1
		end
		startindex = startindex + mesh.count_points
	end
	
puts "		}"

#Exporting UVs

oldway = false

  if oldway
    vertices = pointcount
  else
    maps = genAttrMap( meshes, Proc.new { |mesh, ind| mesh.uv_at( ind, 1 ) } )
    vmap = maps[ 0 ]
    mmap = maps[ 1 ]
    vertices = mmap.size
  end


PRWSU2ASE.status_bar("Group being exported: " + grname + " - UVs")
puts "		*MESH_NUMTVERTEX #{vertices}"
puts "		*MESH_TVERTLIST {"

  if oldway then
  	po = 0
  	for mesh in meshes
  		
  		for p in (1..mesh.count_points)
  			
#			if mat.texture and mat==nil
#				texsize = Geom::Point3d.new(mat.texture.width, mat.texture.height, 1)
#			else
  				texsize = Geom::Point3d.new(1,1,1)
#			end

  			uv = [mesh.uv_at(p,1).x/texsize.x, mesh.uv_at(p,1).y/texsize.y, mesh.uv_at(p,1).z/texsize.z]
  			puts "			*MESH_TVERT #{po}    #{"%.4f" %(uv[0])}    #{"%.4f" %(uv[1])}    0.0000"
  			po = po + 1

  		end
  	end
  else
    for ind in ( 0..mmap.length - 1 )
      uv = mmap[ ind ]
 			puts "			*MESH_TVERT #{ind}    #{"%.4f" %(uv[0])}    #{"%.4f" %(uv[1])}    0.0000"
    end
  end
    
puts "		}"
puts "		*MESH_NUMTVFACES #{polycount}"
puts "		*MESH_TFACELIST {"
	
	startindex = 0
	po = 0
	for mesh in meshes
		for poly in mesh.polygons
			v1 = (poly[0]>=0?poly[0]:-poly[0])+startindex
			v2 = (poly[1]>=0?poly[1]:-poly[1])+startindex
			v3 = (poly[2]>=0?poly[2]:-poly[2])+startindex

      v1 -= 1
      v2 -= 1
      v3 -= 1
        
      if ! oldway
        v1 = vmap[ v1 ]
        v2 = vmap[ v2 ]
        v3 = vmap[ v3 ]
      end
      
			if !mirrored[mesh]
				puts "			*MESH_TFACE   #{po}    #{v1}    #{v2}    #{v3}"
			else
				puts "			*MESH_TFACE   #{po}    #{v2}    #{v1}    #{v3}"
			end
			po = po + 1
		end
		startindex = startindex + mesh.count_points
	end
	
puts "		}"


#Exporting normals

PRWSU2ASE.status_bar("Group being exported: " + grname + " - Normals")
puts "		*MESH_NORMALS {"

	startindex = 0
	po = 0
	for mesh in meshes
		for poly in mesh.polygons
			a1 = (poly[0]>=0?poly[0]:-poly[0])
			a2 = (poly[1]>=0?poly[1]:-poly[1])
			a3 = (poly[2]>=0?poly[2]:-poly[2])
			v1 = a1+startindex
			v2 = a2+startindex
			v3 = a3+startindex
			pos1 = mesh.point_at(a1).to_a
			pos2 = mesh.point_at(a2).to_a
			pos3 = mesh.point_at(a3).to_a
			n1 = mesh.normal_at(a1).to_a
			n2 = mesh.normal_at(a2).to_a
			n3 = mesh.normal_at(a3).to_a
			
			pos2[0] = pos2[0] - pos1[0]
			pos2[1] = pos2[1] - pos1[1]
			pos2[2] = pos2[2] - pos1[2]
			pos3[0] = pos3[0] - pos1[0]
			pos3[1] = pos3[1] - pos1[1]
			pos3[2] = pos3[2] - pos1[2]
			
			pos1[0] = (pos2[1]*pos3[2]) - (pos2[2]*pos3[1])
			pos1[1] = (pos2[2]*pos3[0]) - (pos2[0]*pos3[2])
			pos1[2] = (pos2[0]*pos3[1]) - (pos2[1]*pos3[0])
			
			if !mirrored[mesh]
				puts "			*MESH_FACENORMAL #{po}   #{"%.4f" %(pos1[0])} #{"%.4f" %(pos1[1])} #{"%.4f" %(pos1[2])}"
				puts "				*MESH_VERTEXNORMAL #{v1-1} #{"%.4f" %(n1[0])} #{"%.4f" %(n1[1])} #{"%.4f" %(n1[2])}"
				puts "				*MESH_VERTEXNORMAL #{v2-1} #{"%.4f" %(n2[0])} #{"%.4f" %(n2[1])} #{"%.4f" %(n2[2])}"
        puts "				*MESH_VERTEXNORMAL #{v3-1} #{"%.4f" %(n3[0])} #{"%.4f" %(n3[1])} #{"%.4f" %(n3[2])}"
			else
				puts "			*MESH_FACENORMAL #{po}   #{"%.4f" %(-pos1[1])} #{"%.4f" %(-pos1[0])} #{"%.4f" %(-pos1[2])}"
				puts "				*MESH_VERTEXNORMAL #{v2-1} #{"%.4f" %(-n2[0])} #{"%.4f" %(-n2[1])} #{"%.4f" %(-n2[2])}"
				puts "				*MESH_VERTEXNORMAL #{v1-1} #{"%.4f" %(-n1[0])} #{"%.4f" %(-n1[1])} #{"%.4f" %(-n1[2])}"
        puts "				*MESH_VERTEXNORMAL #{v3-1} #{"%.4f" %(-n3[0])} #{"%.4f" %(-n3[1])} #{"%.4f" %(-n3[2])}"
			end
			po = po + 1
		end
		startindex = startindex + mesh.count_points
	end
	
puts "		}"

puts "	}"
puts "	*PROP_MOTIONBLUR 0"
puts "	*PROP_CASTSHADOW 1"
puts "	*PROP_RECVSHADOW 1"
puts "	*MATERIAL_REF #{$g_prwsu2ase_mat2id[getCurrentMaterial]}"
puts "}"

mirrored={}
twoSided=[]
meshes = []
end

def PRWSU2ASE::reload
  load ("prw-su2ase.rb")
end

end #Class end

# ---------------------------------------- Menu item
if( not file_loaded?("prw-su2ase.rb") )
  $g_prwsu2ase_export_selected=true
  $g_prwsu2ase_tex_ext = "dds"
  $g_prwsu2ase_mat_diffuse = Sketchup::Color.new(255, 255, 255)
  $g_prwsu2ase_write_hidden=false
  $g_prwsu2ase_write_all_materials=false
  $g_prwsu2ase_write_solo_object_name=true

	main_menu = UI.menu("Plugins").add_submenu("PRW ASE Exporter")
	main_menu.add_item("Export") { (PRWSU2ASE.export) }
  main_menu.add_item("Preferences") { (PRWSU2ASE.prefs) }
	main_menu.add_separator
	main_menu.add_item("About PRW SU2ASE") { (PRWSU2ASE.about) }
  main_menu.add_item("Reload SU2ASE") { (PRWSU2ASE.reload) }

end
file_loaded("prw-su2ase.rb")
