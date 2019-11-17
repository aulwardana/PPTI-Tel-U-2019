package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
	"text/template"
)

var (
	cnf        *Config
	configPath string
)

type FileScanData struct {
	Directory string `json:"dir"`
	FileName  string `json:"name"`
}

var allFileScan []FileScanData

var templateHome = template.Must(template.ParseFiles("index.html"))

func initConfig() error {
	flag.StringVar(&configPath, "c", "config.yaml", "Configuration File")
	flag.Parse()

	c, err := NewCfg(configPath)
	if err != nil {
		return err
	}
	cnf = c

	return err
}

func FilePathWalkDir(root string) ([]string, error) {
	var files []string
	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if !info.IsDir() {
			if filepath.Ext(path) == ".mp4" {
				files = append(files, path)
			}
			return nil
		}
		return nil
	})
	return files, err
}

func HomeTemplate(w http.ResponseWriter, r *http.Request) {
	root := cnf.VideoCfg().Dir

	files, err := FilePathWalkDir(root)
	if err != nil {
		panic(err)
	}

	for _, file := range files {
		//fmt.Println(file)

		reg := regexp.MustCompile(cnf.VideoCfg().Dir)
		res := reg.ReplaceAllString(file, "${1}")

		fmt.Println(res)

		//allFileScan = append(allFileScan, FileScanData{Directory: cnf.VideoCfg().Dir, FileName: res})
		data := FileScanData{Directory: cnf.VideoCfg().Dir, FileName: res}

		templateHome.Execute(w, data)
	}
}

func main() {
	err := initConfig()
	if err != nil {
		log.Fatal(err)
	}

	fs := http.FileServer(http.Dir(cnf.HttpCfg().Dir))
	http.Handle("/static/", http.StripPrefix("/static/", fs))

	http.HandleFunc("/", HomeTemplate)

	log.Println("Listening...")
	port := fmt.Sprintf(":%d", cnf.HttpCfg().Port)
	http.ListenAndServe(port, nil)
}
