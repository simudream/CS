#!/usr/bin/env python

import types
from string import *;
import re

try:
	import cPickle
	pickle=cPickle
except:
	print "WARNING: Failed to import module cPickle, using slower pickle library"
	import pickle

try:
	import cspacec
	from cspace import *
except:
    	print "WARNING: Failed to import module cspace, can only pickle the map"

class Object:
	"""Lowest level Unreal Object"""
	def __init__(self, attrib):
	        self.attrib={}
	        self.stack=[]
	        self.Add(attrib)
	def Add(self, attrib):
		self.attrib.update(attrib)
	def Push(self, child):
		self.stack.append(child)
class Actor(Object):
	"""An actor object from Unreal"""
	pass
class Info(Actor):
	pass
class ZoneInfo(Info):
	pass
class LevelInfo(ZoneInfo):
    	pass
class Light(Actor):
    	pass
class PlayerStart(Actor):
    	"""Defines the starting location for an Unreal Map"""
	def Add(self, attrib):
		self.attrib.update(attrib)
		if(attrib.has_key('Location')):
			loc=re.match('X=(.*),Y=(.*),Z=(.*)').groups()
	def Load(self, view):
#		view.GetCamera().SetPosition(csVector3(loc[0],loc[1],loc[2]))
		pass
class Brush(Actor):
    	"""A geometry object from Unreal"""
    	def MakeGeom(self, vecs, room, tm):
		if not self.attrib.has_key('CsgOper'):
			poly=room.NewPolygon(tm)
#		elif self.attrib['CsgOper']=='CSG_Add':
#			thing=csThing()
#			poly=thing.NewPolygon(tm)
#			vecs.Load(poly)
#			room.AddThing(thing)
#			return
		else:
			print "WARNING: Unknown "+self.attrib['CsgOper']
			return
		vecs.Load(poly)
	def Load(self, room, tm):
		for x in self.stack:
			for y in x.stack:
				for z in y.stack:
					self.MakeGeom(z, room, tm)
class FakeBrush(Object):
	pass
class PolyList(Object):
    	"""A list of polygons"""
	pass
	
scale=0.10
	
class Polygon(Object):
    	"""A polygons"""
	def __init__(self, attrib):
	        self.attrib=attrib
	        self.veclist=[]
	def AddPoint(self, vec):
		self.veclist.insert(0, vec)
	def Load(self, poly):
		for x in self.veclist:
			poly.AddVertex(csVector3(x[0], x[1], x[2]))
		poly.SetTextureSpace(poly.Vobj(0), poly.Vobj(1), 3);
	def __repr__(self):
		return str(self.veclist)
	
class Map(Actor):
	"""A pickleable map"""
	def Count(self):
		print "----- Map Count -----"
		print "---------------------"
	def Load(self):
		system=GetSystem()
		room=csSector(roomptr)
		view=csView(viewptr)
		tm=csTextureHandle(tmptr)
		for x in self.stack:
			if isinstance(x, Brush):
				x.Load(room, tm)
			elif isinstance(x, PlayerStart):
				x.Load(view)	
		g3d=system.Query_iGraphics3D()

# The Map Reader

def VectorConvert(vec):
	return (float(vec[0])*scale, float(vec[1])*scale, float(vec[2])*scale)
class UnrealMap:
	"""Import an Unreal Map and pickle it"""
	def Load(self, file):
		if(type(file)==types.StringType):
			file=open(file, 'r')
		self.stack=[]
		self.file=file;
		self.Loop()
		self.map=self.block
	def Save(self, file):
		if(type(file)==types.StringType):
			file=open(file, 'w+')
		pickle.dump(self.map, file)
	def Parse(self):
		self.NextLine()
		if(not len(self.line)):
			return 0
		self.words=split(self.line)
		if(self.words[0]=='Begin'):
			self.Push()
			self.Begin()
			return 1
		if(self.words[0]=='End'):
			self.Pop()
			return 0
		self.block.Add(self.Params(0))
		return 1
	def NextLine(self):
		self.line=self.file.readline()
	def Loop(self):
		while(self.Parse()):
			pass
	def Begin(self):
		if(self.words[1]=='Map'):
			attrib=self.Params(2)
			self.block=Map(attrib)
			print "Found Map", attrib
			self.Loop()
			return
		if(self.words[1]=='Actor'):
			self.Actor()
			return
		if(self.words[1]=='Brush'):
			self.block=FakeBrush(self.Params(2))
			self.Loop()
			return
		if(self.words[1]=='PolyList'):
			self.block=PolyList(self.Params(2))
			self.Loop()
			return
		if(self.words[1]=='Polygon'):
			self.Polygon()
			return
		print "WARNING: Ignoring Unknown: ", self.words[1]
		self.block=None
		self.Loop()
	def Polygon(self):
		self.block=Polygon(self.Params(2))
		while(self.PolyAdd()):
			pass
		self.Pop()
	def PolyAdd(self):
		self.NextLine();
		self.words=split(self.line)
		if(self.words[0]=='Vertex'):
			vecs=split(self.words[1], ',')
			self.block.AddPoint(VectorConvert(vecs))
			return 1
		if(self.words[0]=='End'):
			return 0
		return 1
	def Actor(self):
		attrib=self.Params(2)
		try:
			exec 'actor='+attrib['Class']+'(attrib)'
			self.block=actor
		except:
			print 'ERROR: Actor '+attrib['Class']+' not found'
			self.block=Actor(attrib)
		self.Loop()
	def Pop(self):
		if(len(self.stack)):
			self.stack[0].Push(self.block)
			self.block=self.stack[0]
			del self.stack[0]
	def Push(self):
		if(hasattr(self, 'block')):
			self.stack.insert(0, self.block)
	def Params(self, index):
		params={}
		for x in self.words[index:]:
			word=split(x, '=')
			if(len(word)==2):
				params[word[0]]=word[1]
		return params

def Load(file):
#   	file='data/extremegen'
#    	file='data/bluff'
    	filepym=file+'.pym'
    	filet3d=file+'.t3d'
    	try:
    		print 'Unrmap: Opening '+filepym
    		asdf
		a=pickle.load(open(filepym,'r'))
	except:
		print 'Unrmap: File not openable, attempting conversion of '+filet3d
	 	m=UnrealMap()
		m.Load(filet3d)
		m.Save(filepym)
		a=m.map
	a.Count()
	print 'Unrmap: Map loaded, creating...'
	a.Load()
				
#Load('entry')
