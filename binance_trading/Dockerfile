# set base image (host OS)
FROM python:3.8

# set the working directory in the container
WORKDIR /code

# copy the dependencies file to the working directory
COPY requirements.txt .

RUN apt-get -y update &&\
apt-get install -y supervisor
# install dependencies
RUN pip install -r requirements.txt requests python-binance 

# copy the content of the local src directory to the working directory
COPY sendprice.py . 
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

RUN mkdir -p /var/log/supervisor
CMD ["/usr/bin/supervisord"]

# command to run on container start
#CMD [ "python", "./sendprice.py" ]